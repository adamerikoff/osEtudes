using Printf

function midpoint_integration(x_start, x_end, func, n)
    # Calculate the width of each subinterval
    width = (x_end - x_start) / n
    # Initialize the sum for the integral
    total_area = 0.0
    # Loop over each subinterval
    for i in 0:(n-1)
        # Calculate the midpoint of the current subinterval
        midpoint = x_start + (i + 0.5) * width
        # Evaluate the function at the midpoint
        height = func(midpoint)
        # Add the area of the rectangle to the total area
        total_area += height * width
    end
    return total_area
end

function y(x)
    return sin(x) + cos(5x) + exp(-x)
end

function y_antiderivative(x)
    return -cos(x) + sin(5x)/5 - exp(-x)
end

println(y(0.5))

# Example usage of midpoint_integration
x_start = 0.0
x_end = 6.0
n = 1000
integral_value = midpoint_integration(x_start, x_end, y, n)
@printf("The integral of y(x) from %.2f to %.2f using midpoint integration is approximately %.5f\n", x_start, x_end, integral_value)

# Analytical result
analytical_value = y_antiderivative(x_end) - y_antiderivative(x_start)
@printf("The analytical integral of y(x) from %.2f to %.2f is %.5f\n", x_start, x_end, analytical_value)

# Compare the results
difference = abs(integral_value - analytical_value)
@printf("The difference between numerical and analytical results is %.5f\n", difference)
