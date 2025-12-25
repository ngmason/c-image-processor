This is code for an image filter c program that I built for my SER 334 Operating System & System Program 
class assignment.The BMPHandler.c and Image.c files along with their header files represent classes that handle 
the copying and duplication of .bmp files as well as filters such as apply black and white filter, apply colorshift, 
and apply resize image. You can run these filters with the MasonImageProcessor.c main file in command line using the
following structure for the different filters: "./MasonImageProcessor ttt.bmp -r 56" or 
"./ImageProcessor ttt.bmp -w -r 56 -b 78 -g 45 -o filtered2.bmp" or "./ImageProcessor ttt.bmp -s 2.0 -o filtered4.bmp".
The general structure is also explained in the notes of the MasonImageProcess.c file. 

You can also run the BMPHandler.c and the Image.c classes along with their header files with MasonFilters.c. This is a
multithreaded program that will run a swiss cheese filter on a specified .bmp image in command line. You can run the 
following command in commandline after compiling the classes with MasonFilters.c: "./MasonFilters -i ttt.bmp -o swissttt.bmp -f b" 
which will run a blur filter on the image, or "./MasonFilters -i ttt.bmp -o swissttt.bmp -f c" which will apply the swiss cheese 
filter to the input image.

Code was written and tested in clion as well as command line on a Linux Xubuntu 24 OS, using a Virtual Machine.