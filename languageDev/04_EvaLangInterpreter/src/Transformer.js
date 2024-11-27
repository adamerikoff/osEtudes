class Transformer {

    /**
     * Transforms a function declaration (`def`-expression) into a variable declaration 
     * with a lambda (anonymous function) expression.
     * This allows function definitions to be treated like variables.
     * 
     * Example:
     * Input: ['def', 'myFunc', ['x', 'y'], ['+', 'x', 'y']]
     * Output: ['var', 'myFunc', ['lambda', ['x', 'y'], ['+', 'x', 'y']]]
     */
    transformDefToVarLambda(defExp) {
        const [_, name, params, body] = defExp;
        return ['var', name, ['lambda', params, body]];
    }

    /**
     * Transforms a `switch` statement into a nested series of `if`-expressions.
     * Each case in the switch statement becomes an `if` or `else if` condition.
     *
     * Example:
     * Input: ['switch', ['=', 'x', 10], 100, ['>', 'x', 10], 200, ['else', 300]]
     * Output: ['if', ['=', 'x', 10], 100, ['if', ['>', 'x', 10], 200, 300]]
     */
    transformSwitchToIf(switchExp) {
        const [_, ...cases] = switchExp; 

        const ifExp = ['if', null, null, null]; // Starting structure of the if expression
        let current = ifExp; // Points to the current part of the nested if structure

        for (let i = 0; i < cases.length - 1; i++) {
            const [currentCond, currentBlock] = cases[i];

            current[1] = currentCond; // Condition for this `if`
            current[2] = currentBlock; // Block to execute if the condition is true

            const next = cases[i + 1];
            const [nextCond, nextBlock] = next;

            // If the next case is `else`, use it as the default block; otherwise, continue nesting `if`
            current[3] = nextCond === 'else' ? nextBlock : ['if'];

            // Move to the nested `if` for the next iteration
            current = current[3];
        }

        return ifExp;
    }

    /**
     * Transforms a `for` loop into a `while` loop expression.
     * The initialization happens before the `while`, and the modifier is included 
     * at the end of the loop body.
     *
     * Example:
     * Input: ['for', 'let i = 0', 'i < 10', 'i++', ['console.log(i)']]
     * Output: ['begin', 'let i = 0', ['while', 'i < 10', ['begin', 'i++', ['console.log(i)']]]]
     */
    transformWhileToFor(forExp) {
        const [_, init, condition, modifier, exp] = forExp;

        // Body executes the modifier after each loop iteration
        const body = ['begin', modifier, exp];

        // Initializes the variable, then creates a `while` loop with the condition and modified body
        const whileExp = ['begin', init, ['while', condition, body]];

        return whileExp;
    }

    /**
     * Transforms an increment operation (`++ foo`) into an explicit assignment 
     * that adds 1 to the variable.
     *
     * Example:
     * Input: ['++', 'foo']
     * Output: ['set', 'foo', ['+', 'foo', 1]]
     */
    transformIncToSet(incExp) {
        const [_, exp] = incExp;
        return ['set', exp, ['+', exp, 1]];
    }

    /**
     * Transforms a decrement operation (`-- foo`) into an explicit assignment 
     * that subtracts 1 from the variable.
     *
     * Example:
     * Input: ['--', 'foo']
     * Output: ['set', 'foo', ['-', 'foo', 1]]
     */
    transformDecToSet(decExp) {
        const [_, exp] = decExp;
        return ['set', exp, ['-', exp, 1]];
    }

    /**
     * Transforms an increment-by-value operation (`+= foo val`) into an assignment 
     * that adds a specified value to the variable.
     *
     * Example:
     * Input: ['+=', 'foo', 5]
     * Output: ['set', 'foo', ['+', 'foo', 5]]
     */
    transformIncValToSet(incExp) {
        const [_, exp, val] = incExp;
        return ['set', exp, ['+', exp, val]];
    }

    /**
     * Transforms a decrement-by-value operation (`-= foo val`) into an assignment 
     * that subtracts a specified value from the variable.
     *
     * Example:
     * Input: ['-=', 'foo', 3]
     * Output: ['set', 'foo', ['-', 'foo', 3]]
     */
    transformDecValToSet(decExp) {
        const [_, exp, val] = decExp;
        return ['set', exp, ['-', exp, val]];
    }
};

module.exports = { Transformer };