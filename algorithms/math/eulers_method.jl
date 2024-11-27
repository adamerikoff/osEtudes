# Define the function representing the ODE dy/dx = f(x, y)
function f(x, y)
    return x * y
end

# Implement the Euler method
function euler_method(f, x0, y0, h, n)
    x_values = [x0]
    y_values = [y0]
    x = x0
    y = y0
    
    for i in 1:n
        y += h * f(x, y)
        x += h
        push!(x_values, x)
        push!(y_values, y)
    end
    
    return x_values, y_values
end

# Initial conditions and parameters
x0 = 0.0        # Initial value of x
y0 = 1.0        # Initial value of y(x0)
h = 0.1         # Step size
n = 10          # Number of iterations

# Solve the ODE using Euler method
x_values, y_values = euler_method(f, x0, y0, h, n)

# Output the results
println("Euler Method Approximation:")
for i in 1:length(x_values)
    println("x = $(x_values[i]), y = $(y_values[i])")
end
