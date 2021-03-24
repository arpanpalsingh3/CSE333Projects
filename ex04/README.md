Build using my memmove like this:
      gcc -g -Wall -Wextra -std=gnu99 -DMEMMOVE=memmove_new  memmove.c

Build using standard library memmove like this;
      gcc -g -Wall -Wextra -std=gnu99 -DMEMMOVE=memmove  memmove.c


Performance of my implementation vs. standard library:
   <D
    My implementation worked slower than the library implementation
    for the most part. I called timer.c from the provided source, 
    and essentially timed specifically how long each function took.
    I did 10k calls to each of these 2 functions and took the avearge time
    My function (memmove_new) took roughly 0.096618302 micro seconds on average per call
    The library function (memmove) took roughly  0.036384489 micro seconds on average per call
    The library function is on average about .06 micro seconds (us) faster

    My testing is done through main. I just print out the results after timing them (as done in test.c)
    The test itself is pretty much a copy of the test provide, however the key difference being what is 
    timed. In this case, its the function calls, instead of nanosleep.
    
