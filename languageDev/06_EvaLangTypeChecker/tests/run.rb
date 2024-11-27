require_relative '../src/EvaTC'

tests = [
  require_relative './self_eval_test',
  require_relative './math_test',
  require_relative './variable_test',
  require_relative './block_test',
  require_relative './if_test',
  require_relative './while_test',
  require_relative './user_defined_function_test',
  require_relative './built_in_function_test',
  require_relative './lambda_function_test',
  require_relative './alias_test',
  require_relative './class_test',
  require_relative './union_test',
  require_relative './generics_test'
]

# Execute each test
tests.each do |test|
  eva = EvaTC.new
  test.call(eva)
end

puts 'All assertions passed!'
