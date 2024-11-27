require "./Tokenizer.rb"
require "./Parser.rb"
require "./Generator.rb"


RUNTIME = "function multiply(x,y) { return x * y };"
LOG = "console.log(z(2,10));"

tokens = Tokenizer.new(File.read("test.src")).tokenize
tree = Parser.new(tokens).parse
generated = Generator.new.generate(tree)

puts [RUNTIME, generated, LOG].join("\n")
