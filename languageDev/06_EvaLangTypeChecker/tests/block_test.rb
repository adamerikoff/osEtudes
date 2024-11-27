require_relative '../src/type'
require_relative './test_util'

module TypedEvaTest
  def self.run(eva)
    # Block: sequence of expressions.
    test(eva, [
      'begin',
      ['var', 'x', 10],
      ['var', 'y', 20],
      ['+', ['*', 'x', 10], 'y']
    ], Type.number)

    # Block: local scope.
    test(eva, [
      'begin',
      ['var', 'x', 10],
      [
        'begin',
        ['var', 'x', '"hello"'],
        ['+', 'x', '"world"']
      ],
      ['-', 'x', 5]
    ], Type.number)

    # Block: access parent scopes.
    test(eva, [
      'begin',
      ['var', 'x', 10],
      [
        'begin',
        ['var', 'y', 20],
        ['+', 'x', 'y']
      ]
    ], Type.number)

    # Block: variable update.
    test(eva, [
      'begin',
      ['var', 'x', 10],
      [
        'begin',
        ['var', 'y', 20],
        ['set', 'x', ['+', 'x', 'y']]
      ]
    ], Type.number)

    # Additional test with `begin` block.
    test(eva, <<~EVAL, Type.number)
      (begin
        (var x 10)
        (var y 20)
        (+ (* x 10) y))
    EVAL

    exec(eva, '(var x 10)')

    test(eva, <<~EVAL, Type.number)
      (var y 20)
      (+ (* x 10) y)
    EVAL
  end
end
