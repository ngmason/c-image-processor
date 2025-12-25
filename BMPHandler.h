/**
* Implementation of several functions to manipulate BMP files.
*
* Completion time: 15 minutes
*
* @author Nina Mason, Ruben Acuna
* @version 10/29/24
*/
#ifndef BMPHANDLER_H
#define BMPHANDLER_H
#include <stdio.h>
#include "Image.h"
struct BMP_Header {

    char signature[2]; //ID field
    int size; //Size of BMP file
    short reserved1; //Application specific
    short reserved2; //Application specific
    int offset; //Offset where the pixel array can be found
};
struct DIB_Header{

    int size;
    int width;
    int height;
    short planes;
    short bits;
    int compression;
    int sizeImage;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;
};
/**
* Read BMP header of a BMP file.
*
* @param file: A pointer to the file being read
* @param header: Pointer to the destination BMP header
*/
void readBMPHeader(FILE* file, struct BMP_Header* header);
/**
* Write BMP header of a file. Useful for creating a BMP file.
*
* @param file: A pointer to the file being written
* @param header: The header to write to the file
*/
void writeBMPHeader(FILE* file, struct BMP_Header* header);
/**
* Read DIB header from a BMP file.
*
* @param file: A pointer to the file being read
* @param header: Pointer to the destination DIB header
*/
void readDIBHeader(FILE* file, struct DIB_Header* header);
/**
* Write DIB header of a file. Useful for creating a BMP file.
*
* @param file: A pointer to the file being written
* @param header: The header to write to the file
*/
void writeDIBHeader(FILE* file, struct DIB_Header* header);
/**
* Make BMP header based on width and height. Useful for creating a BMP file.
*
* @param header: Pointer to the destination BMP header
* @param width: Width of the image that this header is for
* @param height: Height of the image that this header is for
*/
void makeBMPHeader(struct BMP_Header* header, int width, int height);
/**
* Make new DIB header based on width and height.Useful for creating a BMP file.
*
* @param header: Pointer to the destination DIB header
* @param width: Width of the image that this header is for
* @param height: Height of the image that this header is for
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height);
/**
* Read Pixels from BMP file based on width and height.
*
* @param file: A pointer to the file being read
* @param pArr: Pixel array to store the pixels being read
* @param width: Width of the pixel array of this image
* @param height: Height of the pixel array of this image
*/
void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height);
/**
* Write Pixels from BMP file based on width and height.
*
* @param file: A pointer to the file being read or written
* @param pArr: Pixel array of the image to write to the file
* @param width: Width of the pixel array of this image
* @param height: Height of the pixel array of this image
*/
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height);
#endif
