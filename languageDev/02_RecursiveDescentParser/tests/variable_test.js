module.exports = test => {
    test(
        `
            let x, y;
        `,
        {
            type: "Program",
            body: [
                {
                    type: "VariableStatement",
                    declarations: [
                        {
                            type: "VariableDeclaration",
                            id: {
                                type: "Identifier",
                                name: "x"
                            },
                            init: null,
                        },
                        {
                            type: "VariableDeclaration",
                            id: {
                                type: "Identifier",
                                name: "y"
                            },
                            init: null,
                        },
                    ]
                }
            ]
        },
    );

    test(
        `
            let x, y = 22;
        `,
        {
            type: "Program",
            body: [
                {
                    type: "VariableStatement",
                    declarations: [
                        {
                            type: "VariableDeclaration",
                            id: {
                                type: "Identifier",
                                name: "x"
                            },
                            init: null,
                        },
                        {
                            type: "VariableDeclaration",
                            id: {
                                type: "Identifier",
                                name: "y"
                            },
                            init: {
                                type: "NumericLiteral",
                                value: 22
                            },
                        },
                    ]
                }
            ]
        },
    );
};