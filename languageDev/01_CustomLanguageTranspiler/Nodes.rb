# Define a DefNode structure to represent a function definition in the abstract syntax tree (AST).
# This node stores:
# - `name`: The name of the function (String)
# - `arg_names`: An array of argument names (Array of Strings)
# - `body`: The body of the function, which can be an expression node representing the function's content.
DefNode = Struct.new(:name, :arg_names, :body)

# Define an IntegerNode structure to represent an integer literal in the AST.
# This node stores:
# - `value`: The integer value (Integer), representing a constant in the code.
IntegerNode = Struct.new(:value)

# Define a CallNode structure to represent a function call in the AST.
# This node stores:
# - `name`: The name of the function being called (String)
# - `arg_exprs`: An array of argument expressions (Array of nodes), each representing an expression passed as an argument.
CallNode = Struct.new(:name, :arg_exprs)

# Define a VarRefNode structure to represent a variable reference in the AST.
# This node stores:
# - `value`: The name of the variable being referenced (String).
# In generated JavaScript, this name will be directly used to reference the variable.
VarRefNode = Struct.new(:value)
