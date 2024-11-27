
require_relative './test_util'
require_relative '../src/type'

module TypedEvaTest
  def self.run(eva)
    exec(eva, <<~EVAL)
      (type int number)
      (type ID int)
      (type Index ID)
    EVAL

    test(eva, <<~EVAL, Type.int)
      (def square ((x int)) -> int (* x x))
      (square 2)
    EVAL

    test(eva, <<~EVAL, Type.ID)
      (def promote ((userID ID)) -> ID (+ 1 userID))
      (promote 1)
    EVAL

    test(eva, <<~EVAL, Type.Index)
      (var (x Index) 1)
      x
    EVAL

    test(eva, <<~EVAL, Type.ID)
      x
    EVAL

    test(eva, <<~EVAL, Type.int)
      x
    EVAL

    test(eva, <<~EVAL, Type.number)
      x
    EVAL
  end
end
