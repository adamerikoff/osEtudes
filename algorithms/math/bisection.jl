using Printf  # Import the Printf module to use @sprintf

# Calculate the polynomial value for a given x
function calculate_polynomial_value(x)
    return x^3 + sin(x) - cos(x^3) + x^2 + 1.29
end

# Bisection method to find the root of the polynomial
function bisection_method((x1, x2), tolerance)
    max_width = maximum(length.(["Counter", "x1", "f(x1)", "x2", "f(x2)"])) + 20
    counter = 0  # Initialize the iteration counter
    x_new = 0.0  # Initialize x_new

    # Print the headers for the table
    println("Bisection Method:")
    println(rpad("Counter", max_width), rpad("x1", max_width), rpad("f(x1)", max_width), rpad("x2", max_width), rpad("f(x2)", max_width))

    while true
        # Calculate the polynomial values at x1 and x2
        f1 = calculate_polynomial_value(x1)
        f2 = calculate_polynomial_value(x2)

        # Prepare the current iteration values for printing
        values = [string(counter), @sprintf("%.20f", x1), @sprintf("%.20f", f1), @sprintf("%.20f", x2), @sprintf("%.20f", f2)]
        # Print the current iteration values with right padding
        println(join(rpad(val, max_width) for val in values))

        # Calculate the midpoint of the interval [x1, x2]
        x_new = (x1 + x2) / 2.0
        f_new = calculate_polynomial_value(x_new)

        # Update the interval based on the sign of the function values
        if f1 * f_new < 0
            x2 = x_new
        else
            x1 = x_new
        end

        # Check for convergence
        if abs(x2 - x1) < tolerance || counter > 20000
            println("Converged to: $x_new in $counter iterations.")
            break
        end

        # Increment the counter
        counter += 1
    end

    # Print the final root value and its polynomial value
    println("ROOT IS ", @sprintf("%.20f", x_new))
    println("f($x_new) IS ", @sprintf("%.20f", calculate_polynomial_value(x_new)))
end

# Example usage of the Bisection method
x1 = -1.0  # Set the initial guess for the root (lower bound)
x2 = 1.0   # Set the initial guess for the root (upper bound)
tolerance = 1e-20  # Set the convergence tolerance
bisection_method((x1, x2), tolerance)  # Call the method to find the root
