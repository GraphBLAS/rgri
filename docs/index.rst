.. CatCutifier documentation master file, created by
   sphinx-quickstart on Wed Apr 24 15:19:01 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to RGRI's documentation!
=======================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

:ref:`genindex`

Classes
-------
.. doxygenclass:: grb::matrix
   :members:

Example
~~~~~~~

.. code-block:: C++

   #include <iostream>
   #include <grb/grb.hpp>
   int main(int argc, char** argv) {
     // Create a new matrix, reading in from a file.
     grb::matrix<float, int> a("data/chesapeake.mtx");

     size_t m = a.shape()[0];
     size_t k = a.shape()[1];

     std::cout << "chesapeake.mtx is a " << m << " by " << k << " matrix." << std::endl;

     // Set element 12,9 (row 12, column 9) to 12.
     a[{12, 9}] = 12;

     grb::matrix<float, int> b("data/chesapeake.mtx");

     auto c = grb::multiply(a, b);

     std::cout << "Sum of elements is " << grb::sum(c) << std::endl;

     return 0;
   }

Methods
-------
.. doxygenfunction:: grb::multiply

.. doxygenfunction:: grb::sum
