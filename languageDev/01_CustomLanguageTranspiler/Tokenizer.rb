# Token struct to store type and value of each token
Token = Struct.new(:type, :value)

class Tokenizer
  # Define token types with their corresponding regular expression patterns.
  # Each token is associated with a name (type) and a regular expression to match it.
  TOKEN_TYPES = [
    [:def, /\bdef\b/],            # Keyword 'def' - defines a function
    [:end, /\bend\b/],            # Keyword 'end' - marks the end of a function
    [:identifier, /\b[a-zA-Z]+\b/], # Identifiers - sequences of alphabetic characters
    [:integer, /\b[0-9]+\b/],     # Integers - sequences of one or more digits
    [:oparen, /\(/],              # Open parenthesis '('
    [:cparen, /\)/],              # Close parenthesis ')'
    [:comma, /,/],                # Comma ',' used for separating arguments
  ]

  # Initialize with the code to be tokenized.
  # @param code [String] the raw source code as a string
  def initialize(code)
    @code = code
  end

  # Main method to tokenize the entire code.
  # Iterates through the code, extracting tokens until the code is fully consumed.
  # @return [Array<Token>] an array of tokens extracted from the code
  def tokenize
    tokens = []  # Array to store the resulting tokens
    until @code.empty?  # Continue until the entire code is processed
      tokens << tokenize_one_token  # Extract and add a token
      @code = @code.strip           # Remove any leading whitespace after token extraction
    end
    tokens  # Return the list of tokens
  end

  # Tokenize a single token by matching it with each defined token type.
  # This method looks for the first matching token type based on the regular expressions.
  # @return [Token] a single token object with its type and value
  def tokenize_one_token
    TOKEN_TYPES.each do |type, re|  # Iterate over each defined token type and regex pattern
      re = /\A(#{re})/               # Ensure the pattern matches from the start of the string
      if @code =~ re                 # Check if the current part of the code matches the pattern
        value = $1                   # Capture the matched value
        @code = @code[value.length..-1]  # Remove the matched value from the code
        return Token.new(type, value)  # Return a new Token object with the type and value
      end
    end

    # Raise an error if no token type matches the current part of the code.
    raise RuntimeError.new("Couldn't match the token on #{@code.inspect}!")
  end
end
