# C Image Processor

A command-line image processing application written in C, developed for <i>SER 334: Operating Systems & System Programming</i> at Arizona State University.
The project demonstrates low-level image manipulation, BMP file handling, and multithreaded filtering using pthreads.

## Features
This project supports two execution modes: 
- Single-Image Processing (CLI Filters)
    - Black & white conversion
    - Color shifting (RGB adjustments)
    - Image resizing
- Multithreaded Filters (Parallel Processing)
    - Using MasonFilters.c, the program applies parallel image filters using pthreads, including:
        - Box Blur Filter
        - Swiss Cheese Filter (stylized effect with randomly generated circular holes)  

Both modes operate on 24-bit uncompressed BMP images.

## Project Structure
File structure:
- c-image-processor/
    - BMPHandler.c / BMPHandler.h (BMP file loading & saving)
    - Image.c / Image.h (Core image operations & filters)
    - MasonImageProcessor.c (CLI image processing (single-threaded))
    - MasonFilters.c (Multithreaded filters (pthreads))
    - ReadMe.txt (Original assignment notes)
    - wb.bmp (Sample input image)
    - ttt.bmp (Sample input image)

## Requirements
- Linux environment (tested on Ubuntu / Xubuntu)
- gcc
- pthread library
- math library

**Note:** This project was developed and tested in a Linux VM and WSL (Ubuntu).

## Build Instructions
From the project directory:

### Compile the single-threaded image processor

```bash
gcc MasonImageProcessor.c BMPHandler.c Image.c -o ImageProcessor -lm
```
### Compile the multithreaded filter processor

```bash
gcc MasonFilters.c BMPHandler.c Image.c -o MasonFilters -lm -pthread
```

## Usage:
### Single-Image Processing (CLI):
- Apply filters directly to an image:
```bash
./ImageProcessor input.bmp -r 56
./ImageProcessor input.bmp -w -r 56 -b 78 -g 45 -o output.bmp
./ImageProcessor input.bmp -s 2.0 -o resized.bmp
```
- Example options:
    - **-w** is Black & white
    - **-r**, **-g**, **-b** is RGB color shift
    - **-s** is Scale factor
    - **-o** is Output file

### Multithreaded Filters:
- Apply parallel filters using pthreads:
```bash
./MasonFilters -i input.bmp -o blurred.bmp -f b
./MasonFilters -i input.bmp -o swiss.bmp -f c
```
- Filter options:
    - **-f b** is Box Blur filter
    - **-f c** is Swiss Cheese filter

## Technical Highlights
- Manual parsing and writing of binary BMP files
- Safe memory allocation for image regions
- Column-based data decomposition for parallel execution
- Thread-safe design using pthreads
- Designed to scale across configurable thread counts

## Why This Project Matters:
This project showcases:
- Strong C fundamentals
- Comfort with Linux envrionments
- Multithreaded programming concepts
- Real-world file I/O and memory management