/**
* Reads an input image file and output image file name and different output options
* from the command line arguments. It then performs the different operations on the input image
* and outputs the filtered image to the output file.
*
* Completion time: 90 minutes
*
* @author Nina Mason
* @version 10/29/24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BMPHandler.h"
#include "Image.h"

int main(int argc, char *argv[]) {
    char *outputFile = NULL;
    char *inputFile = NULL;
    struct BMP_Header BMP;
    struct DIB_Header DIB;
    int red = 0, blue = 0, green = 0;
    struct Image* img = NULL;
    struct Pixel** pixels = NULL;


    for (int i = 1; i < argc; i++) {
        if (i == 1) {
            inputFile = argv[i];
            FILE* file_input = fopen(inputFile, "rb");
            if (!file_input) {
                fprintf(stderr, "Error: Could not open input file %s.\n", inputFile);
                return 1;
            }


            readBMPHeader(file_input, &BMP);
            readDIBHeader(file_input, &DIB);


            pixels = (struct Pixel**)malloc(DIB.height * sizeof(struct Pixel*));
            if (!pixels) {
                fprintf(stderr, "Error: Could not allocate memory for pixel array.\n");
                fclose(file_input);
                return 1;
            }

            for (int p = 0; p < DIB.height; p++) {
                pixels[p] = (struct Pixel*)malloc(sizeof(struct Pixel) * DIB.width);
                if (!pixels[p]) {
                    fprintf(stderr, "Error: Could not allocate memory for pixel row.\n");
                    fclose(file_input);
                    return 1;
                }
            }



            readPixelsBMP(file_input, pixels, DIB.width, DIB.height);


            fclose(file_input);


            img = image_create(pixels, DIB.width, DIB.height);
        }
        else if (strcmp(argv[i], "-w") == 0) {
            image_apply_bw(img);
        }
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            red = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-g") == 0 && i + 1 < argc) {
            green = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            blue = atoi(argv[++i]);
        }
        else if(strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            image_apply_resize(img, atof(argv[++i]));
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }


    if(img != NULL && pixels != NULL) {
        image_apply_colorshift(img, red, green, blue);



        if (outputFile) {
            FILE* file_output = fopen(outputFile, "wb");
            if (!file_output) {
                fprintf(stderr, "Error: Could not open output file %s.\n", outputFile);
                return 1;
            }


            struct BMP_Header newBMP;
            struct DIB_Header newDIB;
            makeBMPHeader(&newBMP, image_get_width(img), image_get_height(img));
            writeBMPHeader(file_output, &newBMP);
            makeDIBHeader(&newDIB, image_get_width(img), image_get_height(img));
            writeDIBHeader(file_output, &newDIB);
            writePixelsBMP(file_output, image_get_pixels(img), image_get_width(img),
            image_get_height(img));
            fclose(file_output);
        } else {
            outputFile = "defaultOutput.bmp";
            FILE* file_output = fopen(outputFile, "wb");
            if (!file_output) {
                fprintf(stderr, "Error: Could not open output file %s.\n", outputFile);
                return 1;
            }


            writeBMPHeader(file_output, &BMP);
            writeDIBHeader(file_output, &DIB);
            writePixelsBMP(file_output, image_get_pixels(img), image_get_width(img),
            image_get_height(img));
            fclose(file_output);
        }

        
        image_destroy(img);
    }

    return 0;

}