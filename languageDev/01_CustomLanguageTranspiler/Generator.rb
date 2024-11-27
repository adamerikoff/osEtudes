# Require the Nodes file, which defines the various node structures (e.g., DefNode, CallNode, IntegerNode, VarRefNode)
require "./Nodes.rb"

# The Generator class is responsible for generating JavaScript code from the Abstract Syntax Tree (AST).
class Generator
  # The main method that generates JavaScript code based on the type of AST node provided.
  # @param node [Node] an AST node representing a language construct (e.g., function definition, function call, integer, or variable reference).
  # @return [String] the JavaScript code equivalent for the given node.
  def generate(node)
    # Determine the type of node and generate JavaScript code accordingly
    case node
    when DefNode
      # Handle function definition nodes (DefNode), which represent a function definition with a name, arguments, and body.
      # Generates JavaScript function code in the form: "function <name>(<args>) { return <body> };"
      "function %s(%s) { return %s };" % [
        node.name,                       # The function name
        node.arg_names.join(","),         # Join argument names with commas to form the parameter list
        generate(node.body),              # Recursively generate JavaScript code for the function body
      ]
    when CallNode
      # Handle function call nodes (CallNode), which represent a function call with a name and argument expressions.
      # Generates JavaScript call in the form: "<name>(<arg_exprs>)"
      "%s(%s)" % [
        node.name,                                           # The function name
        node.arg_exprs.map { |expr| generate(expr) }.join(",") # Recursively generate each argument expression and join with commas
      ]
    when VarRefNode
      # Handle variable reference nodes (VarRefNode), which represent a reference to a variable.
      # Simply return the variable name (JavaScript code for variable references is identical to the variable's name).
      node.value
    when IntegerNode
      # Handle integer nodes (IntegerNode), which represent integer constants.
      # Return the integer value as is, since JavaScript syntax for integer literals is the same.
      node.value
    else
      # Raise an error if the node type is not recognized.
      # This ensures that the Generator class only processes known node types.
      raise RuntimeError.new("Unexpected node type: #{node.class} !");
    end
  end
end
