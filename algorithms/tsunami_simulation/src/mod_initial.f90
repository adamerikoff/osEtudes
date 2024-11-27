module mod_initial

  use iso_fortran_env, only: int32, real32  ! Import Fortran data types for integer and real numbers
  implicit none

  private  ! Make all subroutines and functions private by default
  public :: set_gaussian  ! Explicitly make the set_gaussian subroutine available for use outside the module

contains

  ! Subroutine to initialize an array with a Gaussian function
  pure subroutine set_gaussian(x, icenter, decay)
    real(real32), intent(in out) :: x(:)  ! Input/output array x of real32 type, to store the Gaussian profile
    integer(int32), intent(in) :: icenter  ! Center index for the Gaussian profile
    real(real32), intent(in) :: decay  ! Decay factor controlling the width of the Gaussian function
    integer(int32) :: i  ! Loop index for traversing the array x

    ! Initialize the array x with a Gaussian profile
    ! The Gaussian profile is computed using the formula:
    !    f(i) = exp(-decay * (i - icenter)**2)
    ! where icenter is the location of the peak of the Gaussian, and decay controls its width
    do concurrent(i = 1:size(x))  ! Concurrent loop to update each element of x simultaneously
      x(i) = exp(-decay * (i - icenter)**2)  ! Apply the Gaussian formula for each element
    end do
  end subroutine set_gaussian

end module mod_initial
