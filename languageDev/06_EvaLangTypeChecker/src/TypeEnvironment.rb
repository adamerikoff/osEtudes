class TypeEnvironment
  # Creates an environment with the given record.
  def initialize(record = {}, parent = nil)
    @record = record
    @parent = parent
  end

  # Creates a variable with the given name and a type.
  def define(name, type_)
    @record[name] = type_
    type_
  end

  # Returns the type of a defined variable, or raises an error
  # if the variable is not defined.
  def lookup(name)
    resolve(name).record[name]
  end

  # Returns the specific environment in which a variable is defined,
  # or raises an error if the variable is not defined.
  def resolve(name)
    if @record.key?(name)
      return self
    end

    if @parent.nil?
      raise "Variable \"#{name}\" is not defined."
    end

    @parent.resolve(name)
  end

  # Getter for @record
  def record
    @record
  end
end
