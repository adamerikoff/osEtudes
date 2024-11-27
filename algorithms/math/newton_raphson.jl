using Printf  # Import the Printf module to use @sprintf

# Calculate the polynomial value for a given x
function calculate_polynomial_value(x)
    return x^6 - x^4 - x^2 - 1  # Polynomial function: x^6 - x^4 - x^2 - 1
end

# Calculate the derivative of the polynomial for a given x
function calculate_derivative(x)
    return 6*x^5 - 4*x^3 - 2*x  # Derivative of the polynomial: 6x^5 - 4x^3 - 2x
end

# Newton-Raphson method to find the root of the polynomial
function newton_raphson_method(initial_x, tolerance)
    x = initial_x  # Initial guess for the root
    counter = 0  # Initialize the iteration counter

    # Calculate the maximum width for table columns with extra padding
    max_width = maximum(length.(["Counter", "x_old", "f(x_old)", "x_new", "f(new)"])) + 20

    # Print the headers for the table
    println("Newton-Raphson Method:")
    println(rpad("Counter", max_width), rpad("x_old", max_width), rpad("f(x_old)", max_width), rpad("x_new", max_width), rpad("f(new)", max_width))

    # Start the Newton-Raphson iteration loop
    while true
        # Calculate the new value of x using Newton-Raphson formula
        x_new = x - calculate_polynomial_value(x) / calculate_derivative(x)
        # Calculate the polynomial value at the old and new x
        f_x_old = calculate_polynomial_value(x)
        f_x_new = calculate_polynomial_value(x_new)

        # Prepare the current iteration values for printing
        values = [string(counter), @sprintf("%.20f", x), @sprintf("%.20f", f_x_old), @sprintf("%.20f", x_new), @sprintf("%.20f", f_x_new)]
        # Print the current iteration values with right padding
        println(join(rpad(val, max_width) for val in values))

        # Increment the counter
        counter += 1

        # Check for convergence based on the tolerance or stop after 40000 iterations to prevent infinite loops
        if (abs(f_x_new) < tolerance) || counter > 40000
            println("Converged to: $x_new in $counter iterations.")
            break  # Exit the loop if convergence is achieved or iteration limit is reached
        end

        # Update x for the next iteration
        x = x_new
    end

    # Print the final root value
    println("ROOT IS ", @sprintf("%.20f", x))
end

# Example usage of the Newton-Raphson method
initial_x = 2.5  # Set the initial guess for the root
tolerance = 1e-20  # Set the convergence tolerance
newton_raphson_method(initial_x, tolerance)  # Call the method to find the root
