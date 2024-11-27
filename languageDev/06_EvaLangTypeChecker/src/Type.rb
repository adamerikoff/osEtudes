# Type class.
class Type
  attr_reader :name

  def initialize(name)
    @name = name
  end

  # Returns name.
  def get_name
    @name
  end

  # String representation.
  def to_s
    get_name
  end

  # Equals.
  def equals?(other)
    if other.is_a?(Type::Alias)
      return other.equals?(self)
    end

    if other.is_a?(Type::Union)
      return other.equals?(self)
    end

    @name == other.name
  end

  # From string: 'number' -> Type.number
  def self.from_string(type_str)
    if self.respond_to?(type_str)
      return self.send(type_str)
    end

    if type_str.include?('Fn<')
      return Type::Function.from_string(type_str)
    end

    raise "Unknown type: #{type_str}"
  end
end

# Number type.
Type::NUMBER = Type.new('number')

# String type.
Type::STRING = Type.new('string')

# Boolean type.
Type::BOOLEAN = Type.new('boolean')

# Null type.
Type::NULL = Type.new('null')

# Any type.
Type::ANY = Type.new('any')

# Function meta type.
class Type::Function < Type
  attr_reader :param_types, :return_type

  def initialize(name: nil, param_types:, return_type:)
    super(name)
    @param_types = param_types
    @return_type = return_type
    @name ||= get_name
  end

  # Returns name: Fn<return_type<p1, p2, ...>>
  def get_name
    name = "Fn<#{@return_type.get_name}"
    if @param_types.any?
      params = @param_types.map(&:get_name).join(',')
      name += "<#{params}>"
    end
    name + '>'
  end

  # Equals.
  def equals?(other)
    return false unless @param_types.length == other.param_types.length

    @param_types.each_with_index do |param, i|
      return false unless param.equals?(other.param_types[i])
    end

    @return_type.equals?(other.return_type)
  end

  # From string.
  def self.from_string(type_str)
    if Type.respond_to?(type_str)
      return Type.send(type_str)
    end

    # Function type with return and params
    matched = /^Fn<(\w+)<([a-z,\s]+)>>$/.match(type_str)

    if matched
      return_type_str, params_string = matched.captures
      param_types = params_string.split(/,\s*/).map { |param| Type.from_string(param) }
      return Type::Function.new(name: type_str, param_types: param_types, return_type: Type.from_string(return_type_str))
    end

    # Function type with return type only
    matched = /^Fn<(\w+)>$/.match(type_str)

    if matched
      return_type_str = matched[1]
      return Type::Function.new(name: type_str, param_types: [], return_type: Type.from_string(return_type_str))
    end

    raise "Type.Function.from_string: Unknown type: #{type_str}"
  end
end

# Type alias: (type int number)
class Type::Alias < Type
  attr_reader :parent

  def initialize(name:, parent:)
    super(name)
    @parent = parent
  end

  # Equals.
  def equals?(other)
    if @name == other.name
      return true
    end

    @parent.equals?(other)
  end
end

# Class type: (class ...)
# Creates a new TypeEnvironment.
class Type::Class < Type
  attr_reader :super_class, :env

  def initialize(name:, super_class: Type::NULL)
    super(name)
    @super_class = super_class
    @env = TypeEnvironment.new({}, super_class != Type::NULL ? super_class.env : nil)
  end

  # Return field type.
  def get_field(name)
    @env.lookup(name)
  end

  # Equals override.
  def equals?(other)
    return true if self == other

    # Aliases:
    if other.is_a?(Type::Alias)
      return other.equals?(self)
    end

    if @super_class != Type::NULL
      return @super_class.equals?(other)
    end

    false
  end
end

# Union type: (or string number)
class Type::Union < Type
  attr_reader :option_types

  def initialize(name:, option_types:)
    super(name)
    @option_types = option_types
  end

  # This union includes all types.
  def includes_all?(types)
    return false unless types.length == @option_types.length

    types.all? { |type| equals?(type) }
  end

  # Equals.
  def equals?(other)
    return true if self == other

    # Aliases:
    if other.is_a?(Type::Alias)
      return other.equals?(self)
    end

    # Other union:
    if other.is_a?(Type::Union)
      return includes_all?(other.option_types)
    end

    # Anything else:
    @option_types.any? { |t| t.equals?(other) }
  end
end

# Generic function type.
# Generic functions create normal function types when a function is called.
class Type::GenericFunction < Type
  attr_reader :generic_types, :params, :return_type, :body, :env

  def initialize(name: nil, generic_types_str:, params:, return_type:, body:, env:)
    super("#{name || 'lambda'} <#{generic_types_str}>")
    @generic_types = generic_types_str.split(',')
    @params = params
    @return_type = return_type
    @body = body
    @env = env
  end
end
