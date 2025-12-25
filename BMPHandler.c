/**
* Implementation of several functions to manipulate BMP files.
*
* Completion time: 120 minutes
*
* @author Nina Mason
* @version 10/29/24
*/

#include "BMPHandler.h"
#include "Image.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BMP_HEADER_SIZE 14
#define DIB_HEADER_SIZE 40
#define BIT_COUNT 24

/**
* Read BMP header of a BMP file.
*
* @param file: A pointer to the file being read
* @param header: Pointer to the destination BMP header
*/
void readBMPHeader(FILE* file, struct BMP_Header* header) {
    if(file == NULL || header == NULL) {printf("Error in readBMPHeader()\n"); exit(1);}
    fread(&header->signature, sizeof(char)*2, 1, file);
    fread(&header->size, sizeof(int), 1, file);
    fread(&header->reserved1, sizeof(short), 1, file);
    fread(&header->reserved2, sizeof(short), 1, file);
    fread(&header->offset, sizeof(int), 1, file);

}
/**
* Write BMP header of a file. Useful for creating a BMP file.
*
* @param file: A pointer to the file being written
* @param header: The header to write to the file
*/
void writeBMPHeader(FILE* file, struct BMP_Header* header) {
    if(file == NULL || header == NULL) {printf("Error in writeBMPHeader()\n"); exit(1);}
    fwrite(&header->signature, sizeof(char)*2, 1, file);
    fwrite(&header->size, sizeof(int), 1, file);
    fwrite(&header->reserved1, sizeof(short), 1, file);
    fwrite(&header->reserved2, sizeof(short), 1, file);
    fwrite(&header->offset, sizeof(int), 1, file);
}
/**
* Read DIB header from a BMP file.
*
* @param file: A pointer to the file being read
* @param header: Pointer to the destination DIB header
*/
void readDIBHeader(FILE* file, struct DIB_Header* header) {
    if(file == NULL || header == NULL) {printf("Error in readDBIHeader()\n"); exit(1);}
    fread(&header->size, sizeof(int), 1, file);
    fread(&header->width, sizeof(int), 1, file);
    fread(&header->height, sizeof(int), 1, file);
    fread(&header->planes, sizeof(short), 1, file);
    fread(&header->bits, sizeof(short), 1, file);
    fread(&header->compression, sizeof(int), 1, file);
    fread(&header->xPelsPerMeter, sizeof(int), 1, file);
    fread(&header->yPelsPerMeter, sizeof(int), 1, file);
    fread(&header->clrUsed, sizeof(int), 1, file);
    fread(&header->clrImportant, sizeof(int), 1, file);

}
/**
* Write DIB header of a file. Useful for creating a BMP file.
*
* @param file: A pointer to the file being written
* @param header: The header to write to the file
*/
void writeDIBHeader(FILE* file, struct DIB_Header* header) {
    if(file == NULL || header == NULL) {printf("Error in writeDIBHeader()\n"); exit(1);}
    fwrite(&header->size, sizeof(int), 1, file);
    fwrite(&header->width, sizeof(int), 1, file);
    fwrite(&header->height, sizeof(int), 1, file);
    fwrite(&header->planes, sizeof(short), 1, file);
    fwrite(&header->bits, sizeof(short), 1, file);
    fwrite(&header->compression, sizeof(int), 1, file);
    fwrite(&header->xPelsPerMeter, sizeof(int), 1, file);
    fwrite(&header->yPelsPerMeter, sizeof(int), 1, file);
    fwrite(&header->clrUsed, sizeof(int), 1, file);
    fwrite(&header->clrImportant, sizeof(int), 1, file);
}
/**
* Make BMP header based on width and height. Useful for creating a BMP file.
*
* @param header: Pointer to the destination BMP header
* @param width: Width of the image that this header is for
* @param height: Height of the image that this header is for
*/
void makeBMPHeader(struct BMP_Header* header, int width, int height) {
    if(header == NULL) {printf("Error in makeBMPHeader()\n"); exit(1);}
    header->signature[0] = 'B';
    header->signature[1] = 'M';
    header->reserved1 = 0;
    header->reserved2 = 0;
    header->offset = BMP_HEADER_SIZE + DIB_HEADER_SIZE;
    int rowSize = (int)(((BIT_COUNT * width + 31) / 32.0) * 4);
    header->size = header->offset + rowSize * height;
}
/**
* Make new DIB header based on width and height.Useful for creating a BMP file.
*
* @param header: Pointer to the destination DIB header
* @param width: Width of the image that this header is for
* @param height: Height of the image that this header is for
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height) {
    if(header == NULL) {printf("Error in makeDIBHeader()\n"); exit(1);}
    header->size = DIB_HEADER_SIZE;
    header->width = width;
    header->height = height;
    header->planes = 1;
    header->bits = BIT_COUNT;
    header->compression = 0;
    int rowSize = (int)(((BIT_COUNT * width + 31) / 32.0) * 4);
    header->sizeImage = rowSize * height;
    header->xPelsPerMeter = 3780;
    header->yPelsPerMeter = 3780;
    header->clrUsed = 0;
    header->clrImportant = 0;
}
/**
* Read Pixels from BMP file based on width and height.
*
* @param file: A pointer to the file being read
* @param pArr: Pixel array to store the pixels being read
* @param width: Width of the pixel array of this image
* @param height: Height of the pixel array of this image
*/
void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    int rowSize = ((width * 3 + 3) / 4) * 4;
    int paddingSize = rowSize - (width * 3);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char rgb[3];
            fread(rgb, sizeof(unsigned char), 3, file);
            pArr[i][j].red = rgb[0];
            pArr[i][j].green = rgb[1];
            pArr[i][j].blue = rgb[2];
        }
        fseek(file, paddingSize, SEEK_CUR);
    }
}

/**
* Write Pixels from BMP file based on width and height.
*
* @param file: A pointer to the file being read or written
* @param pArr: Pixel array of the image to write to the file
* @param width: Width of the pixel array of this image
* @param height: Height of the pixel array of this image
*/
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    int rowSize = ((width * 3 + 3) / 4) * 4;
    int paddingSize = rowSize - (width * 3);
    unsigned char padding[3] = {0, 0, 0};

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            unsigned char rgb[3];
            rgb[0] = (unsigned char)pArr[i][j].red;
            rgb[1] = (unsigned char)pArr[i][j].green;
            rgb[2] = (unsigned char)pArr[i][j].blue;
            fwrite(rgb, sizeof(unsigned char), 3, file);
        }
        fwrite(padding, sizeof(unsigned char), paddingSize, file);
    }
}