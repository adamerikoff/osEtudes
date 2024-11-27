program tsunami
  use iso_fortran_env, only: int32, real32  ! Import portable integer and real32 data types for the program
  use mod_diff, only: diff => diff_centered  ! Import the `diff_centered` function from the `mod_diff` module for computing the differences
  use mod_initial, only: set_gaussian  ! Import the `set_gaussian` subroutine from the `mod_initial` module to initialize the water height

  implicit none

  ! Declare variables
  integer(int32) :: n  ! Loop variable for the time steps

  ! Parameters for grid size, number of time steps, and physical constants
  integer(int32), parameter :: grid_size = 100  ! Size of the grid (number of points in the x-direction)
  integer(int32), parameter :: num_time_steps = 10000  ! Number of time steps to run the simulation

  real(real32), parameter :: dt = 0.02  ! Time step size in seconds
  real(real32), parameter :: dx = 1  ! Grid spacing (distance between adjacent points in meters)
  real(real32), parameter :: g = 9.8  ! Gravitational acceleration in m/s^2
  real(real32), parameter :: hmean = 10  ! Mean water depth in meters

  ! Arrays to store water height (h) and velocity (u) at each grid point
  real(real32) :: h(grid_size), u(grid_size)

  ! Parameters for Gaussian initialization
  integer(int32), parameter :: icenter = 25  ! Center of the Gaussian blob (index in the grid)
  real(real32), parameter :: decay = 0.02  ! Decay factor controlling the width of the Gaussian profile

  ! Format for printing the output (scientific notation with a certain precision)
  character(*), parameter :: fmt = '(i0,*(1x,es15.8e2))'

  ! Ensure that the grid size, time step, and grid spacing are positive values
  if (grid_size <= 0) stop 'grid_size must be > 0'
  if (dt <= 0) stop 'time step dt must be > 0'
  if (dx <= 0) stop 'grid spacing dx must be > 0'

  ! Initialize the water height `h` using a Gaussian profile centered at `icenter` with decay `decay`
  call set_gaussian(h, icenter, decay)

  ! Initialize the water velocity `u` to zero at all grid points
  u = 0

  ! Output the initial state of the system (time step 0)
  print fmt, 0, h

  ! Time-stepping loop for the simulation
  time_loop: do n = 1, num_time_steps

    ! Compute the new water velocity `u` at the next time step using the finite difference scheme
    ! The equation: u = u - (u * diff(u) + g * diff(h)) / dx * dt
    ! `diff(u)` is the derivative of velocity (spatial gradient), `diff(h)` is the derivative of water height (spatial gradient)
    u = u - (u * diff(u) + g * diff(h)) / dx * dt

    ! Compute the new water height `h` at the next time step using the finite difference scheme
    ! The equation: h = h - diff(u * (hmean + h)) / dx * dt
    ! `diff(u * (hmean + h))` is the derivative of the product of velocity and height
    h = h - diff(u * (hmean + h)) / dx * dt

    ! Print the current state of the water height at the current time step `n`
    print fmt, n, h

  end do time_loop  ! End of the time-stepping loop

end program tsunami
