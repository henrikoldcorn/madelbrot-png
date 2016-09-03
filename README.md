# madelbrot-png
outputs high-res mandelbrot set PNGs

Makes use of lodepng library, written by Lode Vandevenne (lodev.org)
Makes extensive use of code written by Lode Vandevenne (see http://lodev.org/cgtutor/juliamandelbrot.html). 

Not all features actually work (eg. quartic mode).

compile (tested on cygwin on Windows 7):
g++ lodepng.cpp mandelpng.cpp -o mandelpng.exe

Usage example:
./mandel-lodepng.exe 2 4000 3000 128 ./mandelbrot.png 0
