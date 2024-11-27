const { Spec } = require("./Spec");

class Tokenizer {
    /**
     * Initializes the tokenizer with the input string.
     * @param {string} string - The input string to tokenize.
     */
    init(string) {
        this._string = string;
        this._cursor = 0; // Position in the string
    }

    /**
     * Checks if the end of the input string has been reached.
     * @returns {boolean} - True if at the end of the string, false otherwise.
     */
    isEOF() {
        return this._cursor === this._string.length;
    }

    /**
     * Checks if there are more tokens to process.
     * @returns {boolean} - True if there are more tokens, false otherwise.
     */
    hasMoreTokens() {
        return this._cursor < this._string.length;
    }

    /**
     * Retrieves the next token from the input string.
     * @returns {object|null} - The next token with its type and value, or null if no tokens are left.
     * @throws {SyntaxError} - If an unexpected character is encountered.
     */
    getNextToken() {
        // If no more tokens, return null
        if (!this.hasMoreTokens()) {
            return null;
        }

        // Slice remaining input
        const string = this._string.slice(this._cursor);

        // Match input against each regular expression in Spec
        for (const [regexp, tokenType] of Spec) {
            const tokenValue = this._match(regexp, string);

            // If there's no match, move to the next pattern
            if (tokenValue == null) {
                continue;
            }

            // Skip tokens where tokenType is undefined (whitespace, etc.)
            if (tokenType == null) {
                return this.getNextToken();
            }

            // Return the matched token with type and value
            return {
                type: tokenType,
                value: tokenValue,
            };
        }

        // If no patterns matched, throw an error for the unexpected token
        throw new SyntaxError(`Unexpected token "${string[0]}" !`);
    }

    /**
     * Matches the beginning of the string with the given regular expression.
     * Advances the cursor if a match is found.
     * @param {RegExp} regexp - The regular expression to match against.
     * @param {string} string - The current input string slice.
     * @returns {string|null} - The matched string or null if no match.
     */
    _match(regexp, string) {
        const matched = regexp.exec(string);
        
        // If no match, return null
        if (matched == null) {
            return null;
        }

        // Move cursor forward by the length of the match
        this._cursor += matched[0].length;
        return matched[0];
    }
}

module.exports = {
    Tokenizer,
};
