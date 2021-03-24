ONLY IF YOU WANT, have a look at this alternate version
of ex02.c.  It uses the preprocessor in a way that requires
you to really understand preprocessing to get what is going
on here.

I'd say this version is preferable to the standard one, but
the code is pretty artificial to begin with, and many people
wnat to avoid using the preprocessor in any complicated way
because it's so hard to debug.

To build, in this directory:
$ gcc -Wall -g -std=c17 ./ex02-advanced.c ../dumphex.c
