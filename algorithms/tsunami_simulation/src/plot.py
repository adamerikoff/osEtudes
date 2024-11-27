import numpy as np
import matplotlib.pyplot as plt
import matplotlib

# Set up Matplotlib parameters
matplotlib.use('Agg')  # Use 'Agg' backend for non-interactive plotting (useful for saving figures without displaying them)
matplotlib.rcParams.update({'font.size': 16})  # Set the font size for the plots

# Define the file path where the output data is stored
input_file = './output.txt'

# Function to read and process the data from the input file
def load_data(file_path):
    with open(file_path, encoding='utf-8') as file:
        # Read lines and split each line into a list of values
        data = [line.rstrip().split() for line in file.readlines()]
    # Extract time (first column) and height data (all other columns)
    time = [float(line[0]) for line in data]  # Convert time values to floats
    h = np.array([[float(x) for x in line[1:]] for line in data])  # Convert height values to numpy array
    return time, h  # Return time and height data

# Load data from the input file
time, h = load_data(input_file)

# Setup for plotting
x = np.arange(1, h.shape[1] + 1)  # x-axis values, corresponding to grid indices (1 to number of grid points)
time_steps = [0, 25, 50, 75]  # The time steps that we want to plot

# Create a figure with multiple subplots
fig, axes = plt.subplots(4, 1, figsize=(8, 10), sharex=True, sharey=False)  
# 4 rows, 1 column of subplots with shared x-axis and different y-axes

# Plot each subplot
for i, ax in enumerate(axes):
    # Plot the water height for the selected time step (h[time_steps[i], :]) on the current axis
    ax.plot(x, h[time_steps[i], :], 'b-')  # Plot the height as a blue line
    # Fill the area under the curve with a blue color (alpha for transparency)
    ax.fill_between(x, -0.5, h[time_steps[i], :], color='b', alpha=0.4)  # Adjust fill range to -0.5
    ax.grid()  # Add grid to the plot
    ax.set_xlim(1, 100)  # Set x-axis limits from 1 to 100 (grid indices)
    ax.set_ylim(-0.2, 1.2)  # Set y-axis limits to show the height from -0.2 to 1.2 (to match the second script)
    ax.set_ylabel('Height', fontsize=16)  # Label the y-axis with 'Height' and set font size
    ax.set_xticks([25, 50, 75, 100])  # Set specific x-ticks (grid points)
    ax.set_yticks(np.arange(-0.2, 1.4, 0.2))  # Set y-ticks from -0.2 to 1.2 in steps of 0.2 (to match the second script)
    ax.set_title(f'Time step {time_steps[i]:3d}')  # Title for each subplot indicating the time step

# Customize x-axis labels for the bottom subplot (the last subplot)
axes[3].set_xlabel('Spatial grid index', fontsize=16)  # Label the x-axis with 'Spatial grid index' for the bottom plot
# Remove x-axis labels for all other subplots
for ax in axes[:-1]:
    ax.set_xticklabels([])  # Clear x-tick labels for all subplots except the last one

# Save the plot as a high-resolution JPG image
plt.tight_layout()  # Adjust the layout to prevent overlap between subplots
plt.savefig('output.jpg', format='jpg', dpi=300)  # Save the figure with a resolution of 300 dpi
plt.close(fig)  # Close the figure to free up memory
