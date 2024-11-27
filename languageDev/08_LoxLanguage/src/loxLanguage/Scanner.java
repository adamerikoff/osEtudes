package loxLanguage;

import java.util.ArrayList;
import java.util.HashMap;

public class Scanner {
    private static final HashMap<String, TokenType> keywords = new HashMap<>();

    static {
        keywords.put("and", TokenType.AND);
        keywords.put("class", TokenType.CLASS);
        keywords.put("else", TokenType.ELSE);
        keywords.put("false", TokenType.FALSE);
        keywords.put("for", TokenType.FOR);
        keywords.put("fun", TokenType.FUN);
        keywords.put("if", TokenType.IF);
        keywords.put("nil", TokenType.NIL);
        keywords.put("or", TokenType.OR);
        keywords.put("print", TokenType.PRINT);
        keywords.put("return", TokenType.RETURN);
        keywords.put("super", TokenType.SUPER);
        keywords.put("this", TokenType.THIS);
        keywords.put("true", TokenType.TRUE);
        keywords.put("var", TokenType.VAR);
        keywords.put("while", TokenType.WHILE);
    }

    private final String source;
    private ArrayList<Token> tokens = new ArrayList<>();
    private int start = 0;
    private int current = 0;
    private int currentLine = 1;

    Scanner(String source) {
        this.source = source;
    }

    ArrayList<Token> scanTokens() {
        while (!isEnd()) {
            start = current;
            scanToken();
        }

        tokens.add(new Token(TokenType.EOF, "", null, this.currentLine));
        return tokens;
    }

    private boolean isEnd() {
        return current >= this.source.length();
    }

    private void scanToken() {
        current++;
        char c = this.source.charAt(current - 1);

        switch (c) {
            case '(': addToken(TokenType.LEFT_PAREN); break;
            case ')': addToken(TokenType.RIGHT_PAREN); break;
            case '{': addToken(TokenType.LEFT_BRACE); break;
            case '}': addToken(TokenType.RIGHT_BRACE); break;
            case ',': addToken(TokenType.COMMA); break;
            case '.': addToken(TokenType.DOT); break;
            case '-': addToken(TokenType.MINUS); break;
            case '+': addToken(TokenType.PLUS); break;
            case ';': addToken(TokenType.SEMICOLON); break;
            case '*': addToken(TokenType.STAR); break;

            case '!':
                addToken(match('=') ? TokenType.BANG_EQUAL : TokenType.BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType.EQUAL_EQUAL : TokenType.EQUAL);
                break;
            case '>':
                addToken(match('=') ? TokenType.GREATER_EQUAL :
                                 TokenType.GREATER);
                break;
            case '<':
                addToken(match('=') ? TokenType.LESS_EQUAL :
                                 TokenType.LESS);
                break;
            case '/':
                if (match('/')) {
                    while (!isEnd() && peek() != '\n') {
                        this.current++;
                    }
                } else {
                    addToken(TokenType.SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                this.currentLine++;
                break;
            case '"':
                string();
                break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    Lox.error(this.currentLine, "Unexpected token: " + c);
                }
                break;
        }
    }

    private boolean match(char c) {
        if (isEnd() || this.source.charAt(this.current) != c) return false;
        this.current++;
        return true;
    }

    private char peek() {
        if (isEnd()) return '\0';
        return this.source.charAt(this.current);
    }

    private void string() {
        while (!isEnd() && peek() != '"') {
            if (peek() == '\n') this.currentLine++;
            this.current++;
        }
        if (isEnd()) {
            Lox.error(this.currentLine, "unterminated string");
            return;
        }
        String value = source.substring(this.start + 1, this.current);
        addToken(TokenType.STRING, value);

        this.current++;
    }

    private void number() {
        while (!isEnd() && isDigit(peek())) {
            this.current++;
        }

        if (peek() == '.' && isDigit(peekNext())) {
            this.current++;
            while (!isEnd() && isDigit(peek())) {
                this.current++;
            }
        }

        double value = Double.parseDouble(source.substring(
                this.start,
                this.current
        ));
        addToken(TokenType.NUMBER, value);
    }

    private void identifier() {
        while (!isEnd() && isAlphaNumeric(peek())) {
            this.current++;
        }

        String lexeme = this.source.substring(start, current);
        TokenType type = keywords.getOrDefault(lexeme, TokenType.IDENTIFIER);
        addToken(type);
    }

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    private char peekNext() {
        if (this.current + 1 > this.source.length()) return '\0';
        return this.source.charAt(this.current + 1);
    }

    private void addToken(TokenType token) {
        String lexeme = this.source.substring(start, current);
        tokens.add(new Token(token, lexeme, null, this.currentLine));
    }

    private void addToken(TokenType token, Object literal) {
        String lexeme = this.source.substring(start, current);
        tokens.add(new Token(token, lexeme, literal, this.currentLine));
    }
}

