module mod_diff

  use iso_fortran_env, only: int32, real32  ! Import Fortran data types for integer and real numbers
  implicit none

  private  ! Make all subroutines and functions private by default
  public :: diff_upwind, diff_centered  ! Explicitly make these two functions available for use outside the module

contains

  ! Centered Difference Function for Numerical Derivative
  pure function diff_centered(x) result(dx)
    real(real32), intent(in) :: x(:)  ! Input array x of real32 type, with size defined by the caller
    real(real32) :: dx(size(x))  ! Output array dx to store the result of the derivative
    integer(int32) :: im  ! Integer to store the size of the array x
    
    im = size(x)  ! Get the size (number of elements) of the input array x
    
    ! Central difference for the first element: difference between the second and last element
    dx(1) = x(2) - x(im)
    
    ! Central difference for the last element: difference between the first and second-to-last element
    dx(im) = x(1) - x(im-1)
    
    ! Central difference for the interior elements: difference between the next and previous elements
    dx(2:im-1) = x(3:im) - x(1:im-2)
    
    ! Multiply the differences by 0.5 to compute the centered difference
    dx = 0.5 * dx
  end function diff_centered

  ! Upwind Difference Function for Numerical Derivative
  pure function diff_upwind(x) result(dx)
    real(real32), intent(in) :: x(:)  ! Input array x of real32 type, with size defined by the caller
    real(real32) :: dx(size(x))  ! Output array dx to store the result of the derivative
    integer(int32) :: im  ! Integer to store the size of the array x
    
    im = size(x)  ! Get the size (number of elements) of the input array x
    
    ! Upwind difference for the first element: difference between the first and last element
    dx(1) = x(1) - x(im)
    
    ! Upwind difference for the rest of the elements: difference between each element and its previous element
    dx(2:im) = x(2:im) - x(1:im-1)
  end function diff_upwind

end module mod_diff
