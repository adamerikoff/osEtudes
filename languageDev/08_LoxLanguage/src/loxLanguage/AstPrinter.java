package loxLanguage;

public class AstPrinter implements Expr.Visitor < String > {
    String print(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public String visitBinaryExpr(Expr.Binary binary) {
        return parenthesize(binary.operator.lexeme, binary.left, binary.right);
    }

    @Override
    public String visitCallExpr(Expr.Call call) {
        return null;
    }

    @Override
    public String visitGetExpr(Expr.Get Get) {
        return null;
    }

    @Override
    public String visitSetExpr(Expr.Set Set) {
        return null;
    }

    @Override
    public String visitSuperExpr(Expr.Super Super) {
        return null;
    }

    @Override
    public String visitThisExpr(Expr.This This) {
        return null;
    }

    @Override
    public String visitGroupingExpr(Expr.Grouping grouping) {
        return parenthesize("group", grouping.expression);
    }

    @Override
    public String visitLiteralExpr(Expr.Literal literal) {
        if (literal == null) return "nil";
        return literal.value.toString();
    }

    @Override
    public String visitUnaryExpr(Expr.Unary unary) {
        return parenthesize(unary.operator.lexeme, unary.right);
    }

    @Override
    public String visitVariableExpr(Expr.Variable variable) {
        return null;
    }

    @Override
    public String visitAssignExpr(Expr.Assign assign) {
        return null;
    }

    @Override
    public String visitLogicalExpr(Expr.Logical logical) {
        return null;
    }

    private String parenthesize(String name, Expr...expressions) {
        StringBuilder builder = new StringBuilder();
        builder.append("(").append(name);

        for (Expr expr: expressions) {
            builder.append(" ");
            builder.append(expr.accept(this));
        }
        builder.append(")");

        return builder.toString();
    }
}
