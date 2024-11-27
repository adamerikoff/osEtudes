class Environment {
    // Constructor method to initialize the environment.
    // Accepts a record object (default is an empty object) to store variables
    // and an optional parent environment, allowing for nested scopes.
    constructor(record = {}, parent = null) {
        this.record = record;  // Stores variables in the current environment
        this.parent = parent;  // References the parent environment, if any
    }

    // Method to create a new variable in the current environment.
    // Takes a variable name and its value, then adds it to the record.
    // Returns the assigned value.
    define(name, value) {
        this.record[name] = value; // Stores the variable in the current record
        return value; // Returns the value for convenience
    }

    // Method to update an existing variable in the appropriate environment.
    // Searches for the variable in the current and parent environments, 
    // then updates its value in the closest scope where it is defined.
    assign(name, value) {
        // Finds the environment where the variable is defined and updates it there.
        // This affects only the closest (nearest) environment where the variable is found.
        this.resolve(name).record[name] = value;
        return value; // Returns the value for confirmation
    }

    // Method to retrieve the value of a defined variable.
    // Searches for the variable in the current and parent environments.
    // If found, returns its value; if not found, throws an error.
    lookup(name) {
        return this.resolve(name).record[name]; // Accesses the variable's value
    }

    // Helper method to locate the specific environment in which a variable is defined.
    // Checks the current environment first, and if not found,
    // recursively checks parent environments.
    // Throws an error if the variable is not defined in any environment.
    resolve(name) {
        // Checks if the variable exists in the current record
        if (this.record.hasOwnProperty(name)) {
            return this; // Returns the current environment if variable is found
        }

        // If there's no parent environment and the variable isn't in the current record,
        // throws a ReferenceError because the variable is undefined.
        if (this.parent == null) {
            throw new ReferenceError(`Variable "${name}" is not defined.`);
        }

        // Recursively checks the parent environment if the variable isn't in the current one
        return this.parent.resolve(name);
    }
}

module.exports = { Environment };