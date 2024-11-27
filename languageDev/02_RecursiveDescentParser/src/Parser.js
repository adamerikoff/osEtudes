const { Tokenizer } = require("./Tokenizer");
const { DefaultFactory, SExpressionFactory } = require("./Factories");

// Choose the AST mode
const AST_MODE = "default";
const factory = AST_MODE === "default" ? DefaultFactory : SExpressionFactory;

class Parser {
    constructor() {
        this._string = "";
        this._tokenizer = new Tokenizer();
    }

    // Main entry point for parsing a string
    parse(string) {
        this._string = string;
        this._tokenizer.init(string);
        this._lookahead = this._tokenizer.getNextToken();
        return this.Program();
    }

    // Parse a program, which consists of statements
    Program() {
        return factory.Program(this.StatementList());
    }

    // Parse a list of statements (terminated by a stopLookahead)
    StatementList(stopLookahead = null) {
        const statementList = [this.Statement()];
        while (this._lookahead != null && this._lookahead.type !== stopLookahead) {
            statementList.push(this.Statement());
        }
        return statementList;
    }

    // Determine the type of statement based on the lookahead token
    Statement() {
        switch (this._lookahead.type) {
            case ";":
                return this.EmptyStatement();
            case "if":
                return this.IfStatement();
            case "{":
                return this.BlockStatement();
            case "let":
                return this.VariableStatement();
            case "def":
                return this.FunctionDeclaration();
            case "class":
                return this.ClassDeclaration();
            case "return":
                return this.ReturnStatement();
            case "while":
            case "do":
            case "for":
                return this.IterationStatement();
            default:
                return this.ExpressionStatement();
        }
    }

    ClassDeclaration() {
        this._eat("class");
        const id = this.Identifier();
        const superClass = this._lookahead.type === "extends" ? this.ClassExtends() : null;
        const body = this.BlockStatement();
        return factory.ClassDeclaration(id, superClass, body);
    }

    ClassExtends() {
        this._eat("extends");
        return this.Identifier();
    }

    FunctionDeclaration() {
        this._eat("def");
        const name = this.Identifier();
        this._eat("(");
        const params = this._lookahead.type !== ")" ? this.FormalParameterList() : [];
        this._eat(")");
        const body = this.BlockStatement();
        return factory.FunctionDeclaration(name, params, body);
    }
    
    FormalParameterList() {
        const params = [];
        do {
            params.push(this.Identifier());
        } while (this._lookahead.type === "," && this._eat(","));
        return params;
    }

    ReturnStatement() {
        this._eat("return");
        const argument = this._lookahead.type !== ";" ? this.Expression() : null;
        this._eat(";");
        return factory.ReturnStatement(argument);
    }

    IterationStatement() {
        switch (this._lookahead.type) {
            case "while":
                return this.WhileStatement();
            case "do":
                return this.DoWhileStatement();
            case "for":
                return this.ForStatement();
        }
    }

    WhileStatement() {
        this._eat("while");
        this._eat("(");
        const test = this.Expression();
        this._eat(")");
        const body = this.Statement();
        return factory.WhileStatement(test, body);
    }

    DoWhileStatement() {
        this._eat("do");
        const body = this.Statement();
        this._eat("while");
        this._eat("(");
        const test = this.Expression();
        this._eat(")");
        this._eat(";");
        return factory.DoWhileStatement(test, body);
    }

    ForStatement() {
        this._eat("for");
        this._eat("(");
        const init = this._lookahead.type !== ";" ? this.ForStatementInit() : null;
        this._eat(";");
        const test = this._lookahead.type !== ";" ? this.Expression() : null;
        this._eat(";");
        const update = this._lookahead.type !== ")" ? this.Expression() : null;
        this._eat(")");
        const body = this.Statement();
        return factory.ForStatement(init, test, update, body);
    }

    ForStatementInit() {
        if (this._lookahead.type == "let") {
            return this.VariableStatementInit();
        }
        return this.Expression();
    }

    // Parse an empty statement (just a semicolon)
    EmptyStatement() {
        this._eat(";");
        return factory.EmptyStatement();
    }

    // Parse an 'if' statement with optional 'else' part
    IfStatement() {
        this._eat("if");
        this._eat("(");
        const test = this.Expression();
        this._eat(")");
        const consequent = this.Statement();
        let alternate = null;
        if (this._lookahead != null && this._lookahead.type === "else") {
            this._eat("else");
            alternate = this.Statement();
        }

        return factory.IfStatement(test, consequent, alternate);
    }

    // Parse a block statement
    BlockStatement() {
        this._eat("{");
        const body = this._lookahead.type !== "}" ? this.StatementList("}") : [];
        this._eat("}");
        return factory.BlockStatement(body);
    }

    // Parse a variable declaration statement
    VariableStatementInit() {
        this._eat("let");
        const declarations = this.VariableDeclarationList();
        return factory.VariableStatement(declarations);
    }

    VariableStatement() {
        const variableStatement = this.VariableStatementInit();
        this._eat(";");
        return variableStatement;
    }

    // Parse a list of variable declarations (separated by commas)
    VariableDeclarationList() {
        const declarations = [];
        do {
            declarations.push(this.VariableDeclaration());
        } while (this._lookahead.type === "," && this._eat(","));
        return declarations;
    }

    // Parse a single variable declaration
    VariableDeclaration() {
        const id = this.Identifier();
        const init = this._lookahead && this._lookahead.type === "SIMPLE_ASSIGN" 
            ? this.VariableInitializer() 
            : null;
        return factory.VariableDeclaration(id, init);
    }

    // Parse the initializer for a variable
    VariableInitializer() {
        this._eat("SIMPLE_ASSIGN");
        return this.AssignmentExpression();
    }

    // Parse an expression statement (e.g., an expression followed by a semicolon)
    ExpressionStatement() {
        const expression = this.Expression();
        this._eat(";");
        return factory.ExpressionStatement(expression);
    }

    // Expression parsing entry point (handles assignment expressions)
    Expression() {
        return this.AssignmentExpression();
    }

    // Parse an assignment expression (handles assignment operators)
    AssignmentExpression() {
        const left = this.LogicalORExpression();
        if (!this._isAssignmentOperator(this._lookahead.type)) {
            return left;
        }
        const operator = this.AssignmentOperator().value;
        const right = this.AssignmentExpression();
        return factory.AssignmentExpression(operator, this._checkValidAssignmentTarget(left), right);
    }

    // Parse logical OR expressions (handles "||" operator)
    LogicalORExpression() {
        return this._LogicalExpression("LogicalANDExpression", "LOGICAL_OR");
    }

    // Parse logical AND expressions (handles "&&" operator)
    LogicalANDExpression() {
        return this._LogicalExpression("EqualityExpression", "LOGICAL_AND");
    }

    // Helper for parsing logical expressions (AND/OR)
    _LogicalExpression(builderName, operatorToken) {
        let left = this[builderName]();
        while (this._lookahead.type === operatorToken) {
            const operator = this._eat(operatorToken).value;
            const right = this[builderName]();
            left = factory.LogicalExpression(operator, left, right);
        }
        return left;
    }

    // Parse binary expressions for addition and subtraction
    AdditiveExpression() {
        return this._BinaryExpression("MultiplicativeExpression", "ADDITIVE_OPERATOR");
    }

    // Parse binary expressions for multiplication and division
    MultiplicativeExpression() {
        return this._BinaryExpression("UnaryExpression", "MULTIPLICATIVE_OPERATOR");
    }

    // Helper for parsing binary expressions (e.g., arithmetic operations)
    _BinaryExpression(builderName, operatorToken) {
        let left = this[builderName]();
        while (this._lookahead != null && this._lookahead.type === operatorToken) {
            const operator = this._eat(operatorToken).value;
            const right = this[builderName]();
            left = factory.BinaryExpression(operator, left, right);
        }
        return left;
    }

    // Parse equality expressions (handles "==" and "!=")
    EqualityExpression() {
        return this._BinaryExpression("RelationalExpression", "EQUALITY_OPERATOR");
    }

    // Parse relational expressions (handles "<", ">", "<=", ">=")
    RelationalExpression() {
        return this._BinaryExpression("AdditiveExpression", "RELATIONAL_OPERATOR");
    }

    // Parse unary expressions (e.g., negation or logical NOT)
    UnaryExpression() {
        let operator;
        switch (this._lookahead.type) {
            case "ADDITIVE_OPERATOR":
                operator = this._eat("ADDITIVE_OPERATOR").value;
                break;
            case "LOGICAL_NOT":
                operator = this._eat("LOGICAL_NOT").value;
                break;
        }
        if (operator != null) {
            const argument = this.UnaryExpression();
            return factory.UnaryExpression(operator, argument);
        }
        return this.LeftHandSideExpression();
    }

    LeftHandSideExpression() {
        return this.CallMemberExpression();
    }

    CallMemberExpression() {
        if (this._lookahead.type === "super") {
            return this._CallExpression(this.Super());
        }
        const member = this.MemberExpression();
        if (this._lookahead.type === "(") {
            return this._CallExpression(member);
        }
        return member;
    }

    _CallExpression(callee) {
        let callExpression = factory.CallExpression(callee, this.Arguments());
        if (this._lookahead.type === "(") {
            callExpression = this._CallExpression(callExpression);
        }
        return callExpression;
    }

    Arguments() {
        this._eat("(");
        const argumentList = this._lookahead.type !== ")" ? this.ArgumentList() : [];
        this._eat(")");
        return argumentList;
    }

    ArgumentList() {
        const argumentList = [];
        do {
            argumentList.push(this.AssignmentExpression());
        } while (this._lookahead.type === "," && this._eat(","));
        return argumentList;
    }

    MemberExpression() {
        let object = this.PrimaryExpression();
        while (this._lookahead.type === "." || this._lookahead.type === "[") {
            if (this._lookahead.type === ".") {
                this._eat(".");
                const property = this.Identifier();
                object = factory.MemberExpression(false, object, property);
            }
            if (this._lookahead.type === "[") {
                this._eat("[");
                const property = this.Expression();
                this._eat("]");
                object = factory.MemberExpression(true, object, property);
            }
        }
        return object
    }

    // Parse a primary expression (either literals or identifiers)
    PrimaryExpression() {
        if (this._isLiteral(this._lookahead.type)) {
            return this.Literal();
        }
        switch (this._lookahead.type) {
            case "(":
                return this.ParenthesizedExpression();
            case "IDENTIFIER":
                return this.Identifier();
            case "this":
                return this.ThisExpression();
            case "new":
                return this.NewExpression();
            default:
                return this.LeftHandSideExpression();
        }
    }

    NewExpression() {
        this._eat("new");
        return factory.NewExpression(this.MemberExpression(), this.Arguments());
    }

    ThisExpression() {
        this._eat("this");
        return factory.ThisExpression();
    }

    Super() {
        this._eat("super");
        return factory.Super();
    }
    // Check if a token is a literal (number, string, boolean, null)
    _isLiteral(tokenType) {
        return tokenType === "NUMBER" || tokenType === "STRING" || tokenType === "true" || tokenType === "false" || tokenType === "null";
    }

    // Parse a parenthesized expression (e.g., (expression))
    ParenthesizedExpression() {
        this._eat("(");
        const expression = this.Expression();
        this._eat(")");
        return expression;
    }

    // Parse an identifier (variable or function name)
    Identifier() {
        const name = this._eat("IDENTIFIER").value;
        return factory.Identifier(name);
    }

    // Parse literals (numbers, strings, booleans, null)
    Literal() {
        switch (this._lookahead.type) {
            case "NUMBER":
                return this.NumericLiteral();
            case "STRING":
                return this.StringLiteral();
            case "true":
                return this.BooleanLiteral(true);
            case "false":
                return this.BooleanLiteral(false);
            case "null":
                return this.NullLiteral();
        }
        throw new SyntaxError(`Unexpected literal "${this._lookahead.type}"!`);
    }

    // Parse a numeric literal
    NumericLiteral() {
        const token = this._eat("NUMBER");
        return factory.NumericLiteral(Number(token.value));
    }

    // Parse a string literal
    StringLiteral() {
        const token = this._eat("STRING");
        return factory.StringLiteral(token.value.slice(1, -1));  // Remove quotes
    }

    // Parse a boolean literal (true or false)
    BooleanLiteral(value) {
        this._eat(value ? "true" : "false");
        return factory.BooleanLiteral(value);
    }

    // Parse a null literal
    NullLiteral() {
        this._eat("null");
        return factory.NullLiteral();
    }

    // Helper method to "eat" (consume) a token and move to the next one
    _eat(tokenType) {
        const token = this._lookahead;
        if (token == null) {
            throw new SyntaxError(`Unexpected end of input, expected: "${tokenType}"!`);
        }
        if (token.type !== tokenType) {
            throw new SyntaxError(`Unexpected token: "${token.value}", expected: "${tokenType}"!`);
        }
        this._lookahead = this._tokenizer.getNextToken();
        return token;
    }

    // Check if a token type is an assignment operator
    _isAssignmentOperator(tokenType) {
        return tokenType === "SIMPLE_ASSIGN" || tokenType === "COMPLEX_ASSIGN";
    }

    // Handle assignment operators (simple or complex)
    AssignmentOperator() {
        if (this._lookahead.type === "SIMPLE_ASSIGN") {
            return this._eat("SIMPLE_ASSIGN");
        }
        return this._eat("COMPLEX_ASSIGN");
    }

    // Validate if the assignment target is valid (e.g., an identifier)
    _checkValidAssignmentTarget(node) {
        if (node.type === "Identifier" || node.type === "MemberExpression") {
            return node;
        }
        throw new SyntaxError(`Invalid left-hand side in assignment expression!`);
    }
}

module.exports = {
    Parser,
};
