using Plots

x = range(0, 10, length=100)
y = sin.(x)
display(plot(x, y))
readline()