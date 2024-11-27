// Import necessary modules
const { Environment } = require('./Environment');
const { Transformer } = require('./Transformer');
const evaParser = require('../parser/evaParser');
const fs = require('fs'); // File system module (Node library)

class Eva {
    /*
     * Creates an Eva instance with the global environment.
     * This initializes the Eva interpreter with a default global environment 
     * and a transformer instance to handle expression transformations.
     */
    constructor(global = GlobalEnvironment) {
        this.global = global;
        this._transformer = new Transformer();
    }

    /*
     * Evaluates global code by wrapping it into a block and evaluating it 
     * within the global environment.
     */
    evalGlobal(exp) {
        return this._evalBody(exp, this.global); // Evaluates a 'block' in the global scope
    }

    /*
     * Evaluates an expression within the provided environment.
     * Supports various types of expressions like self-evaluation, block, 
     * variable declaration, update, access, conditionals, loops, etc.
     */
    eval(exp, env = this.global) {
        // ------------------------------------------------------------
        // Self-evaluation expressions for numbers and strings:
        if (this._isNumber(exp)) {
            return exp;
        }

        if (this._isString(exp)) {
            return exp.slice(1, -1); // Strip quotes from the string
        }

        // ------------------------------------------------------------
        // Block expression: sequence of expressions wrapped in 'begin'
        if (exp[0] === 'begin') {
            const blockEnv = new Environment({}, env); // New environment for the block
            return this._evalBlock(exp, blockEnv);
        }

        // ------------------------------------------------------------
        // Variable declaration: 'var' keyword
        if (exp[0] === 'var') {
            const [_, name, value] = exp;
            return env.define(name, this.eval(value, env)); // Define and assign variable
        }

        // ------------------------------------------------------------
        // Variable update: 'set' keyword
        if (exp[0] === 'set') {
            const [_, ref, value] = exp;
            // Check for property assignment on an instance:
            if (ref[0] === 'prop') {
                const [_, instance, propName] = ref;
                const instanceEnv = this.eval(instance, env); // Evaluate the instance
                return instanceEnv.define(propName, this.eval(value, env)); // Update property
            }
            return env.assign(ref, this.eval(value, env)); // Regular variable update
        }

        // ------------------------------------------------------------
        // Variable access:
        if (this._isVariableName(exp)) {
            return env.lookup(exp);
        }

        // ------------------------------------------------------------
        // Conditional expressions: 'if' keyword
        if (exp[0] === 'if') {
            const [_, condition, consequent, alternate] = exp;
            if (this.eval(condition, env)) {
                return this.eval(consequent, env); // Execute consequent if condition is true
            }
            return this.eval(alternate, env); // Otherwise execute alternate
        }

        // ------------------------------------------------------------
        // Switch expression (syntactic sugar for if expression)
        if (exp[0] === 'switch') {
            const ifExp = this._transformer.transformSwitchToIf(exp);
            return this.eval(ifExp, env);
        }

        // ------------------------------------------------------------
        // While loop expressions:
        if (exp[0] === 'while') {
            const [_, condition, body] = exp;
            let result;
            while (this.eval(condition, env)) {
                result = this.eval(body, env); // Repeatedly evaluate the body
            }
            return result;
        }

        // ------------------------------------------------------------
        // For loop (syntactic sugar for while expression with initialization)
        if (exp[0] === 'for') {
            const whileExp = this._transformer.transformWhileToFor(exp);
            return this.eval(whileExp, env);
        }

        // ------------------------------------------------------------
        // Increment/decrement operations: ++, --, +=, -=
        if (exp[0] === '++') {
            const incExp = this._transformer.transformIncToSet(exp);
            return this.eval(incExp, env);
        }
        if (exp[0] === '--') {
            const decExp = this._transformer.transformDecToSet(exp);
            return this.eval(decExp, env);
        }
        if (exp[0] === '+=') {
            const incValExp = this._transformer.transformIncValToSet(exp);
            return this.eval(incValExp, env);
        }
        if (exp[0] === '-=') {
            const decValExp = this._transformer.transformDecValToSet(exp);
            return this.eval(decValExp, env);
        }

        // ------------------------------------------------------------
        // Function declaration with 'def'
        if (exp[0] === 'def') {
            const varExp = this._transformer.transformDefToVarLambda(exp);
            return this.eval(varExp, env); // Transform to var-lambda and evaluate
        }

        // ------------------------------------------------------------
        // Anonymous function (lambda) expression
        if (exp[0] === 'lambda') {
            const [_, params, body] = exp;
            return { params, body, env }; // Return a closure with captured environment
        }

        // ------------------------------------------------------------
        // Class declaration
        if (exp[0] === 'class') {
            const [_, name, parent, body] = exp;
            const parentEnv = this.eval(parent, env) || env; // If no parent, use current env
            const classEnv = new Environment({}, parentEnv); // Class environment

            this._evalBody(body, classEnv); // Evaluate the class body
            return env.define(name, classEnv); // Define the class in current environment
        }

        // ------------------------------------------------------------
        // Superclass reference for inheritance
        if (exp[0] === 'super') {
            const [_, className] = exp;
            return this.eval(className, env).parent; // Return the parent class
        }

        // ------------------------------------------------------------
        // Class instantiation with 'new' keyword
        if (exp[0] === 'new') {
            const classEnv = this.eval(exp[1], env); // Get class environment
            const instanceEnv = new Environment({}, classEnv); // New instance environment

            const args = exp.slice(2).map(arg => this.eval(arg, env)); // Arguments for constructor
            this._callUserDefinedFunction(classEnv.lookup('constructor'), [instanceEnv, ...args]);
            return instanceEnv;
        }

        // ------------------------------------------------------------
        // Property access within an instance
        if (exp[0] === 'prop') {
            const [_, instance, name] = exp;
            const instanceEnv = this.eval(instance, env); // Evaluate instance environment
            return instanceEnv.lookup(name); // Lookup property
        }

        // ------------------------------------------------------------
        // Module declaration
        if (exp[0] === 'module') {
            const [_, name, body] = exp;
            const moduleEnv = new Environment({}, env); // New environment for the module
            this._evalBody(body, moduleEnv); // Evaluate module body
            return env.define(name, moduleEnv); // Define module in current environment
        }

        // ------------------------------------------------------------
        // Module import from a file
        if (exp[0] === 'import') {
            const [_, name] = exp;
            const moduleSrc = fs.readFileSync(`${__dirname}/stdlib/${name}.eva`, 'utf8');
            const body = evaParser.parse(`(begin ${moduleSrc})`); // Parse module code
            const moduleExp = ['module', name, body]; // Wrap in module expression
            return this.eval(moduleExp, this.global); // Evaluate the module
        }

        // ------------------------------------------------------------
        // Function call handling
        if (Array.isArray(exp)) {
            const fn = this.eval(exp[0], env); // Function to call
            const args = exp.slice(1).map(arg => this.eval(arg, env)); // Evaluate arguments
            if (typeof fn === 'function') {
                return fn(...args); // Native function call
            }
            return this._callUserDefinedFunction(fn, args); // User-defined function call
        }

        throw `Unimplemented: ${JSON.stringify(exp)}`;
    }

    /*
     * Calls a user-defined function by creating a new environment for its parameters.
     */
    _callUserDefinedFunction(fn, args) {
        const activationRecord = {}; // Storage for parameters
        fn.params.forEach((param, index) => {
            activationRecord[param] = args[index]; // Bind arguments to parameters
        });

        const activationEnv = new Environment(activationRecord, fn.env); // New function scope
        return this._evalBody(fn.body, activationEnv); // Evaluate function body
    }

    /*
     * Evaluates a body of expressions, wrapping it as a block if necessary.
     */
    _evalBody(body, env) {
        if (body[0] === 'begin') {
            return this._evalBlock(body, env);
        }
        return this.eval(body, env); // Single expression evaluation
    }

    /*
     * Evaluates a sequence of expressions in a block.
     */
    _evalBlock(block, env) {
        let result;
        const [_, ...expressions] = block;
        expressions.forEach(exp => {
            result = this.eval(exp, env); // Sequential evaluation
        });
        return result;
    }

    /*
     * Checks if an expression is a number.
     */
    _isNumber(exp) {
        return typeof exp === 'number';
    }

    /*
     * Checks if an expression is a string.
     */
    _isString(exp) {
        return typeof exp === 'string' && exp[0] === '"' && exp.slice(-1) === '"';
    }

    /*
     * Checks if an expression is a valid variable name.
     */
    _isVariableName(exp) {
        return typeof exp === 'string' && /^[+\-*/<>=a-zA-Z0-9_]*$/.test(exp);
    }
}

/*
 * Default Global Environment with basic values and functions
 */
const GlobalEnvironment = new Environment({
    null: null,
    true: true,
    false: false,
    VERSION: '0.1',

    // Arithmetic operations:
    '+'(op1, op2) {
        return op1 + op2;
    },
    '*'(op1, op2) {
        return op1 * op2;
    },
    '-'(op1, op2 = null) {
        return op2 == null ? -op1 : op1 - op2;
    },
    '/'(op1, op2) {
        return op1 / op2;
    },

    // Comparison operators:
    '>'(op1, op2) {
        return op1 > op2;
    },
    '<'(op1, op2) {
        return op1 < op2;
    },
    '>='(op1, op2) {
        return op1 >= op2;
    },
    '<='(op1, op2) {
        return op1 <= op2;
    },
    '='(op1, op2) {
        return op1 === op2;
    },

    // Console output function:
    print(...args) {
        console.log(...args);
    },
});


module.exports = { Eva };