This is a reference implementation of the following KEMs (selected in
paramsmenu.h):

   sntrup653
   sntrup761
   sntrup857
   ntrulpr653
   ntrulpr761
   ntrulpr857

This implementation is designed primarily for clarity, subject to the
following constraints:

   * The implementation is written in C. We have a separate Sage
     implementation that is considerably more concise even though it
     includes many more internal self-tests.

   * The implementation avoids data-dependent branches and array
     indices. For example, conditional swaps are computed by arithmetic
     rather than by branches.

   * The implementation avoids other C operations that often take
     variable time. For example, divisions by 3 are computed via
     multiplications and shifts.
     
This implementation does _not_ sacrifice clarity for speed. On the
contrary, this implementation has been rewritten to be even easier to
understand, with function structure matching the Sage implementation.

This rewrite has not yet been reviewed for correctness or for
constant-time behavior. It does pass various tests and has no known
bugs, but there are at least some platforms where multiplications take
variable time, and fixing this requires platform-specific effort; see
https://www.bearssl.org/ctmul.html and http://repository.tue.nl/800603.
