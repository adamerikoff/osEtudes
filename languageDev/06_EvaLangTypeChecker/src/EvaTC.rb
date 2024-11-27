require './type'
require './type_environment'

class EvaTC
  def initialize
    @global = create_global
  end

  def tc_global(exp)
    tc_body(exp, @global)
  end

  def tc(exp, env = @global)
    # Self-evaluating:
    if is_number?(exp)
      return Type.number
    elsif is_string?(exp)
      return Type.string
    elsif is_boolean?(exp)
      return Type.boolean
    elsif is_binary?(exp)
      return binary(exp, env)
    elsif is_boolean_binary?(exp)
      return boolean_binary(exp, env)
    elsif exp[0] == 'type'
      return handle_type_declaration(exp, env)
    elsif exp[0] == 'class'
      return handle_class_declaration(exp, env)
    elsif exp[0] == 'new'
      return handle_class_instantiation(exp, env)
    elsif exp[0] == 'super'
      return handle_super_expression(exp)
    elsif exp[0] == 'prop'
      return handle_property_access(exp, env)
    elsif exp[0] == 'var'
      return handle_variable_declaration(exp, env)
    elsif is_variable_name?(exp)
      return env.lookup(exp)
    elsif exp[0] == 'set'
      return handle_variable_update(exp, env)
    elsif exp[0] == 'begin'
      block_env = TypeEnvironment.new({}, env)
      return tc_block(exp, block_env)
    elsif exp[0] == 'if'
      return handle_if_expression(exp, env)
    elsif exp[0] == 'while'
      return handle_while_expression(exp, env)
    elsif exp[0] == 'def'
      return handle_function_declaration(exp, env)
    elsif exp[0] == 'lambda'
      return handle_lambda_function(exp, env)
    elsif exp.is_a?(Array)
      return handle_function_call(exp, env)
    else
      raise "Unknown type for expression #{exp}"
    end
  end

  private

  def handle_type_declaration(exp, env)
    _tag, name, base = exp
    if base[0] == 'or'
      options = base[1..]
      option_types = options.map { |option| Type.from_string(option) }
      Type[name] = Type::Union.new(name, option_types)
    else
      if Type.has_key?(name)
        raise "Type #{name} is already defined #{Type[name]}"
      end
      unless Type.has_key?(base)
        raise "Type #{base} is not defined."
      end
      Type[name] = Type::Alias.new(name: name, parent: Type[base])
    end
  end

  def handle_class_declaration(exp, env)
    _tag, name, super_class_name, body = exp
    super_class = Type[super_class_name]
    class_type = Type::Class.new(name: name, super_class: super_class)
    Type[name] = env.define(name, class_type)
    tc_body(body, class_type.env)
    class_type
  end

  def handle_class_instantiation(exp, env)
    _tag, class_name, *arg_values = exp
    class_type = Type[class_name]
    raise "Unknown class #{class_name}" if class_type.nil?
    arg_types = arg_values.map { |arg| tc(arg, env) }
    check_function_call(class_type.get_field('constructor'), [class_type, *arg_types], env, exp)
  end

  def handle_super_expression(exp)
    _tag, class_name = exp
    class_type = Type[class_name]
    raise "Unknown class #{class_name}" if class_type.nil?
    class_type.super_class
  end

  def handle_property_access(exp, env)
    _tag, instance, name = exp
    instance_type = tc(instance, env)
    instance_type.get_field(name)
  end

  def handle_variable_declaration(exp, env)
    _tag, name, value = exp
    value_type = tc(value, env)
    if name.is_a?(Array)
      var_name, type_str = name
      expected_type = Type.from_string(type_str)
      expect(value_type, expected_type, value, exp)
      env.define(var_name, expected_type)
    else
      env.define(name, value_type)
    end
  end

  def handle_variable_update(exp, env)
    _, ref, value = exp
    if ref[0] == 'prop'
      _tag, instance, prop_name = ref
      instance_type = tc(instance, env)
      value_type = tc(value, env)
      prop_type = instance_type.get_field(prop_name)
      expect(value_type, prop_type, value, exp)
    else
      value_type = tc(value, env)
      var_type = tc(ref, env)
      expect(value_type, var_type, value, exp)
    end
  end

  def handle_if_expression(exp, env)
    _tag, condition, consequent, alternate = exp
    t1 = tc(condition, env)
    expect(t1, Type.boolean, condition, exp)
    consequent_env = env
    if is_type_cast_condition?(condition)
      name, specific_type = get_specified_type(condition)
      consequent_env = TypeEnvironment.new({name => Type.from_string(specific_type)}, env)
    end
    t2 = tc(consequent, consequent_env)
    t3 = tc(alternate, env)
    expect(t3, t2, exp, exp)
  end

  def handle_while_expression(exp, env)
    _tag, condition, body = exp
    t1 = tc(condition, env)
    expect(t1, Type.boolean, condition, exp)
    tc(body, env)
  end

  def handle_function_declaration(exp, env)
    var_exp = transform_def_to_var_lambda(exp)
    unless is_generic_def_function?(exp)
      name = exp[1]
      params = exp[2]
      return_type_str = exp[4]
      param_types = params.map { |param| Type.from_string(param[1]) }
      env.define(name, Type::Function.new(param_types, Type.from_string(return_type_str)))
    end
    tc(var_exp, env)
  end

  def handle_lambda_function(exp, env)
    if is_generic_lambda_function?(exp)
      create_generic_function_type(exp, env)
    else
      create_simple_function_type(exp, env)
    end
  end

  def handle_function_call(exp, env)
    fn = tc(exp[0], env)
    arg_values = exp[1..]
    if fn.is_a?(Type::GenericFunction)
      actual_types = extract_actual_call_types(exp)
      generic_types_map = get_generic_types_map(fn.generic_types, actual_types)
      bound_params, bound_return_type = bind_function_types(fn.params, fn.return_type, generic_types_map)
      actual_fn = tc_function(bound_params, bound_return_type, fn.body, fn.env)
      arg_values = exp[2..]
    else
      actual_fn = fn
    end
    arg_types = arg_values.map { |arg| tc(arg, env) }
    check_function_call(actual_fn, arg_types, env, exp)
  end

  def create_generic_function_type(exp, env)
    _tag, generic_types, params, _ret_del, return_type, body = exp
    Type::GenericFunction.new(
      generic_types_str: generic_types[1..-2],
      params: params,
      body: body,
      return_type: return_type,
      env: env
    )
  end

  def create_simple_function_type(exp, env)
    _tag, params, _ret_del, return_type_str, body = exp
    tc_function(params, return_type_str, body, env)
  end

  def tc_function(params, return_type_str, body, env)
    return_type = Type.from_string(return_type_str)
    params_record = {}
    param_types = params.map do |name, type_str|
      param_type = Type.from_string(type_str)
      params_record[name] = param_type
      param_type
    end
    fn_env = TypeEnvironment.new(params_record, env)
    actual_return_type = tc_body(body, fn_env)
    expect(actual_return_type, return_type, body, exp)
    Type::Function.new(param_types, return_type)
  end

  def create_global
    TypeEnvironment.new({
      "VERSION" => Type.string,
      "sum" => Type.from_string('Fn<number<number,number>>'),
      "square" => Type.from_string('Fn<number<number>>'),
      "typeof" => Type.from_string('Fn<string<any>>')
    })
  end

  def expect(actual_type, expected_type, value, exp)
    raise "Expected '#{expected_type}' type for #{value} in #{exp}, but got '#{actual_type}' type." unless actual_type.equals(expected_type)
    actual_type
  end

  def expect_operator_type(type_, allowed_types, exp)
    if type_.is_a?(Type::Union)
      return if type_.includes_all(allowed_types)
    elsif allowed_types.any? { |t| t.equals(type_) }
      return
    end
    raise "Unexpected type: #{type_} in #{exp}, allowed: #{allowed_types}"
  end

  def binary(exp, env)
    check_arity(exp, 2)
    t1 = tc(exp[1], env)
    t2 = tc(exp[2], env)
    expect_operator_type(t1, [Type.number], exp)
    expect_operator_type(t2, [Type.number], exp)
    Type.number
  end

  def boolean_binary(exp, env)
    check_arity(exp, 2)
    t1 = tc(exp[1], env)
    t2 = tc(exp[2], env)
    expect_operator_type(t1, [Type.boolean], exp)
    expect_operator_type(t2, [Type.boolean], exp)
    Type.boolean
  end

  def check_arity(exp, arity)
    raise "Invalid arity in #{exp}, expected #{arity}" if exp.length - 1 != arity
  end
end
