package loxLanguage;

import java.util.List;

public class LoxFunction implements LoxCallable {
    private final Stmt.Function declaration;
    private final Environment closure;
    private final boolean isInitializer;

    LoxFunction(Stmt.Function declaration, Environment closure) {
        this(declaration, closure, false);
    }

    LoxFunction(Stmt.Function declaration, Environment closure, boolean isInitializer) {
        this.declaration = declaration;
        this.closure = closure;
        this.isInitializer = isInitializer;
    }

    @Override
    public Object call(Interpreter interpreter, List<Object> arguments) {
        Environment local = new Environment(closure);

        for (int i = 0; i < arguments.size(); i++) {
            String paramName = declaration.params.get(i).lexeme;
            Object argument = arguments.get(i);
            local.define(paramName, argument);
        }

        try {
            interpreter.executeBlock(declaration.body, local);
        } catch (Return r) {
            if (!isInitializer) return r.value;
        }

        if (isInitializer) return closure.getAt(0, "this");
        return null;
    }

    public LoxFunction bind(LoxInstance instance) {
        Environment environment = new Environment(closure);
        environment.define("this", instance);
        return new LoxFunction(declaration, environment, isInitializer);
    }

    @Override
    public int arity() {
        return declaration.params.size();
    }

    @Override
    public String toString() {
        return "<fn " + declaration.name.lexeme + ">";
    }
}
