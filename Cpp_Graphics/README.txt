!!!!! This sample is not a demonstration of my best code, it is simply a demonstration of my c++ abilities in particular.
!!!!! If you want to see what I consider more practical and functional code, please look at my other code samples.

Eigen Main Page: http://eigen.tuxfamily.org/index.php?title=Main_Page
File Processing Reference: https://studiofreya.com/2015/09/12/read-and-write-to-file-in-c/#:~:text=C%2B%2B%20read%20file.%20C%2B%2B%20uses%20file%20streams%20for,to%20first%20attach%20the%20stream%20to%20a%20file.
GeneralizedBresenham Reference: https://www.cs.drexel.edu/~david/Classes/CS430/Lectures/L-02_Lines.pdf
CohenSutherland Reference: https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/
SutherlandHodgman Reference: https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm-please-change-bmp-images-jpeg-png/
Scan Line Filling Reference: https://www.geeksforgeeks.org/scan-line-polygon-filling-using-opengl-c/

The language used was C++ and the OS was linux, as the program was created on tux. The compiler is g++.

The program is only 1 file named "main.cpp". 

There is a Makefile included in the project, so simply type "make" in the project's directory to compile, which will generate an executable named "CG_hw5".
You can also remove the executable using "make clean".

Inputs:

These are in the form of [-OPTION] description of option (DEFAULT VALUE)

[-f] Followed by the name of the first SMF model. Its base color is "Maxval 0 0" (red). (bound-sprellpsd.smf)
[-g] Followed by the name of the second SMF model. Its base color is "0 Maxval 0" (green). (no file)
[-i] Followed by the name of the third SMF model. Its base color is "0 0 Maxval " (blue). (no file)
[-j] The next argument is an integer lower bound in the x dimension of the view port in screen coordinates (0)
[-k] The next argument is an integer in lower bound in the y dimension of the view port in screen coordinates (0)
[-o] The next argument is an integer in upper bound in the x dimension of the view port in screen coordinates (500)
[-p] The next argument is an integer in upper bound in the y dimension of the view port in screen coordinates (500)
[-x] floating point x of Projection Reference Point (PRP) in VRC coordinates (0.0)
[-y] floating point y of Projection Reference Point (PRP) in VRC coordinates (0.0)
[-z] floating point z of Projection Reference Point (PRP) in VRC coordinates (1.0)
[-X] floating point x of View Reference Point (VRP) in world coordinates (0.0)
[-Y] floating point y of View Reference Point (VRP) in world coordinates (0.0)
[-Z] floating point z of View Reference Point (VRP) in world coordinates (0.0)
[-q] floating point x of View Plane Normal vector (VPN) in world coordinates (0.0)
[-r] floating point y of View Plane Normal vector (VPN) in world coordinates (0.0)
[-w] floating point z of View Plane Normal vector (VPN) in world coordinates (-1.0)
[-Q] floating point x of View Up Vector (VUP) in world coordinates (0.0)
[-R] floating point y of View Up Vector (VUP) in world coordinates (1.0)
[-W] floating point z of View UP Vector (VUP) in world coordinates (0.0)
[-u] floating point u min of the VRC window in VRC coordinates (-0.7)
[-v] floating point v min of the VRC window in VRC coordinates (-0.7)
[-U] floating point u max of the VRC window in VRC coordinates (0.7)
[-V] floating point v max of the VRC window in VRC coordinates (0.7)
[-F] Followed by the floating point coordinate of the Front (Near) plane in VRC coordinates. (0.6)
[-B] Followed by the floating point coordinate of the Back (Far) plane in VRC coordinates. (-0.6)
[-P] Use parallel projection. If this flag is not present, use perspective projection.

I also included a folder called "pbms" which includes the output for the provided example inputs:

./CG_hw5 -f bound-sprellpsd.smf -j 0 -k 0 -o 500 -p 500 -x 0.0 -y 0.0 -z 1.0 -X 0.0 -Y 0.0 -Z 0.0 -q 0.0 -r 0.0 -w -1.0 -Q 0.0 -R 1.0 -W 0.0 -u -0.7 -v -0.7 -U 0.7 -V 0.7 -F 0.6 -B -0.6 > hw5_a.ppm
./CG_hw5 -f bound-cow.smf -F 0.1 > hw5_b.ppm
./CG_hw5 -f bound-cow.smf -F 0.1 -B -0.18 > hw5_c.ppm
./CG_hw5 -f bound-bunny_1k.smf -g bound-cow.smf -i bound-sprtrd.smf -u -.8 -U .8 -v -.8 -V .8 > hw5_d.ppm
./CG_hw5 -f bound-bunny_1k.smf -g bound-cow.smf -i bound-sprtrd.smf -q .4 -r .2 -w 1 -u -.6 -v -.6 -U .6 -V .6 -P > hw5_e.ppm
./CG_hw5 -j 334 -k 24 -o 449 -p 402 -f bound-sprellpsd.smf > hw5_f.ppm
./CG_hw5 -u -1.0 -U 0.8 -v -0.9 -V 1.0 -g bound-bunny_1k.smf -f bound-sprtrd.smf -j 34 -k 104 -o 149 -p 472 > hw5_g.ppm
./CG_hw5 -u -1.0 -U 0.8 -v -0.9 -V 1.0 -g bound-bunny_1k.smf -f bound-sprtrd.smf -j 34 -k 104 -o 149 -p 472 -Q -1.1 -R 0.9 -W 0.6 > hw5_h.ppm

This program supports:

viewport mapping
parallel projection
perspective projection
line clipping
near/far plane clipping
Z-buffering
depth-cueing
