module.exports = test => {
    test(
        `
            x = 55;
        `,
        {
            type: "Program",
            body: [
                {
                    type: "ExpressionStatement",
                    expression: {
                        type: "AssignmentExpression",
                        operator: "=",
                        left: {
                            type: "Identifier",
                            name: "x",
                        },
                        right: {
                            type: "NumericLiteral",
                            value: 55
                        }
                    }
                }
            ]
        },
    );

    test(
        `
            x = z = 66;
        `,
        {
            type: "Program",
            body: [
                {
                    type: "ExpressionStatement",
                    expression: {
                        type: "AssignmentExpression",
                        operator: "=",
                        left: {
                            type: "Identifier",
                            name: "x",
                        },
                        right: {
                            type: "AssignmentExpression",
                            operator: "=",
                            left: {
                                type: "Identifier",
                                name: "z",
                            },
                            right: {
                                type: "NumericLiteral",
                                value: 66
                            }
                        }
                    }
                }
            ]
        },
    );
};