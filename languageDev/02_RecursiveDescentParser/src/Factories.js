const DefaultFactory = {
    Program(body) {
        return {
            type: "Program",
            body,
        };
    },
    EmptyStatement() {
        return {
            type: "EmptyStatement"
        };
    },
    BlockStatement(body) {
        return {
            type: "BlockStatement",
            body,
        };
    },
    ExpressionStatement(expression) {
        return {
            type: "ExpressionStatement",
            expression,
        };
    },
    VariableStatement(declarations) {
        return {
            type: "VariableStatement",
            declarations
        };
    },
    VariableDeclaration(id, init) {
        return {
            type: "VariableDeclaration",
            id,
            init
        };
    },
    AssignmentExpression(operator, left, right) {
        return {
            type: "AssignmentExpression",
            operator,
            left,
            right
        };
    },
    IfStatement(test, consequent, alternate) {
        return {
            type: "IfStatement",
            test,
            consequent,
            alternate
        };
    },
    LogicalExpression(operator, left, right) {
        return {
            type: "LogicalExpression",
            operator,
            left,
            right
        };
    },
    UnaryExpression(operator, argument) {
        return {
            type: "UnaryExpression",
            operator,
            argument
        };
    },
    Identifier(name) {
        return {
            type: "Identifier",
            name,
        };
    },
    BinaryExpression(operator, left, right) {
        return {
            type: "BinaryExpression",
            operator,
            left,
            right
        };
    },
    StringLiteral(value) {
        return {
            type: "StringLiteral",
            value,
        };
    },
    NumericLiteral(value) {
        return {
            type: "NumericLiteral",
            value,
        };
    },
    BooleanLiteral(value) {
        return {
            type: "BooleanLiteral",
            value
        };
    },
    NullLiteral() {
        return {
            type: "NullLiteral",
            value: null
        };
    },
    WhileStatement(test, body) {
        return {
            type: "WhileStatement",
            test,
            body,
        };
    },
    DoWhileStatement(test, body) {
        return {
            type: "DoWhileStatement",
            test,
            body
        };
    },
    ForStatement(init, test, update, body) {
        return {
            type: "ForStatement",
            init,
            test,
            update,
            body
        };
    },
    FunctionDeclaration(name, params, body) {
        return {
            type: "FunctionDeclaration",
            name,
            params,
            body,
        };
    },
    ReturnStatement(argument) {
        return {
            type: "ReturnStatement",
            argument
        }
    },
    MemberExpression(computed, object, property) {
        return {
            type: "MemberExpression",
            computed,
            object,
            property,
        };
    },
    CallExpression(callee, arguments) {
        return {
            type: "CallExpression",
            callee,
            arguments,
        };
    },
    ClassDeclaration(id, superClass, body) {
        return {
            type: "ClassDeclaration",
            id,
            superClass,
            body,
        };
    },
    ThisExpression() {
        return {
            type: "ThisExpression",
        };
    },
    Super() {
        return {
            type: "Super",
        };
    },
    NewExpression(callee, arguments) {
        return {
            type: "NewExpression",
            callee,
            arguments
        };
    },
};

const SExpressionFactory = {
    Program(body) {
        return ["begin", body];
    },
    EmptyStatement() {
        return ["empty"];
    },
    BlockStatement(body) {
        return ["begin", body];
    },
    ExpressionStatement(expression) {
        return expression;
    },
    VariableStatement(declarations) {
        return ["var", declarations];
    },
    VariableDeclaration(id, init) {
        return init ? ["assign", id, init] : ["var", id];
    },
    AssignmentExpression(operator, left, right) {
        return [operator, left, right];
    },
    IfStatement(test, consequent, alternate) {
        return ["if", test, consequent, alternate || null];
    },
    LogicalExpression(operator, left, right) {
        return [operator, left, right];
    },
    UnaryExpression(operator, argument) {
        return [operator, argument];
    },
    Identifier(name) {
        return name;
    },
    BinaryExpression(operator, left, right) {
        return [operator, left, right];
    },
    StringLiteral(value) {
        return `"${value}"`;
    },
    NumericLiteral(value) {
        return value;
    },
    BooleanLiteral(value) {
        return value ? "true" : "false";
    },
    NullLiteral() {
        return "null";
    },
    WhileStatement(test, body) {
        return ["while", test, body];
    },
    DoWhileStatement(test, body) {
        return ["do", body, "while", test];
    },
    ForStatement(init, test, update, body) {
        return ["for", init, test, update, body];
    },
};

module.exports = { DefaultFactory, SExpressionFactory };
