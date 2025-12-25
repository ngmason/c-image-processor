/**
* Threaded program that loads an image file, applies a filter to it, and saves it back to the disk.
*
* Completion time: 7 hours
*
* @author Mason
* @version 11/13/2024
*/
////////////////////////////////////////////////////////////////////////////////
//INCLUDES
#include <stdio.h>
//TODO: finish me
//UNCOMMENT BELOW LINE IF USING SER334 LIBRARY/OBJECT FOR BMP SUPPORT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include "BMPHandler.h"
#include "Image.h"
////////////////////////////////////////////////////////////////////////////////
//MACRO DEFINITIONS
//problem assumptions
#define MAX_HOLES 350
#define BMP_HEADER_SIZE 14
#define BMP_DIB_HEADER_SIZE 40
#define MAXIMUM_IMAGE_SIZE 4096
#define THREAD_COUNT 4

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES
struct threadData {
    Image* inImage; //input image part
    Image* outImage; //output image part
    int startCol; //starting point in the row
    int endCol; //ending point in the row
    int width; //width of image part
    int height; //height of image part
    int numHoles;
};

//Struct that stores hole information for the swiss cheese filter.
struct hole {
    int xorigin;
    int yorigin;
    int radius;
};

//Global hole struct array for the holes in an image.
struct hole holes[MAX_HOLES];
////////////////////////////////////////////////////////////////////////////////
//MAIN PROGRAM CODE
//Function definitions.
/**
 * Takes the height and width of an image and computes the x, y origin pixels
 * and radius for a random number of black circles. Stores them in a global struct hole array.
 * @param height
 * @param width
 * @return integer that is the number of holes created for an image.
 */
int computeHoles(int height, int width) {
    int holeCount = 0; // Use global `holes` array directly
    int minDim = (height < width) ? height : width;
    int aveRadius = minDim * 0.08;
    int numHoles = minDim * 0.08;
    int adjustAmount = 5;

    for (int n = 0; n < numHoles; n++) {
        int Xorigin = rand() % (width + 1);
        int Yorigin = rand() % (height + 1);
        int radius;

        int randomValue = rand() % 4;
        if (randomValue == 1) {
            radius = aveRadius + adjustAmount;
        } else if (randomValue == 2) {
            radius = aveRadius - adjustAmount;
        } else {
            radius = aveRadius;
        }

        // Store the hole in the global array
        holes[holeCount].xorigin = Xorigin;
        holes[holeCount].yorigin = Yorigin;
        holes[holeCount].radius = radius;
        holeCount++;
    }

    return holeCount;
}
/**
 * Helper function to allocate a padded sub-region of an image for a thread.
 * @param img
 * @param startCol
 * @param endCol
 * @return sub image of img
 */
Image* allocate_padded_sub_image(Image* img, int startCol, int endCol) {
    int paddedWidth = endCol - startCol + 2;
    Image* subImg = (Image*)malloc(sizeof(Image));
    subImg->width = paddedWidth;
    subImg->height = img->height;
    subImg->pArr = (struct Pixel**)malloc(sizeof(struct Pixel*) * img->height);

    for (int i = 0; i < img->height; i++) {
        subImg->pArr[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * paddedWidth);

        for (int j = 0; j < paddedWidth; j++) {
            int actualCol = startCol + j - 1;
            if (actualCol >= 0 && actualCol < img->width) {
                subImg->pArr[i][j] = img->pArr[i][actualCol];
            } else {

                subImg->pArr[i][j] = (struct Pixel){0, 0, 0};
            }
        }
    }
    return subImg;
}
/**
 * Helper function to allocate a sub-region of an image for a thread.
 * @param img
 * @param startCol
 * @param endCol
 * @return sub image of img
 */
Image* allocate_sub_image(Image* img, int startCol, int endCol) {
    int subWidth = endCol - startCol;
    Image* subImg = (Image*)malloc(sizeof(Image));
    subImg->width = subWidth;
    subImg->height = img->height;

    subImg->pArr = (struct Pixel**)malloc(sizeof(struct Pixel*) * img->height);
    for (int i = 0; i < img->height; i++) {
        subImg->pArr[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * subWidth);

        for (int j = 0; j < subWidth; j++) {
            subImg->pArr[i][j] = img->pArr[i][startCol + j];
        }
    }
    return subImg;
}
/**
 * Helper function to allocate am ouput image for a thread.
 * @param height
 * @param subWidth
 * @return new output image
 */
Image* allocate_thread_out_image(int height, int subWidth) {
    Image* outImg = (Image*)malloc(sizeof(Image));
    struct Pixel** newpix = (struct Pixel**)malloc(sizeof(struct Pixel*) * height);
    for (int i = 0; i < height; i++) {
        newpix[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * subWidth);
    }
    outImg = image_create(newpix, subWidth, height);
    return outImg;
}

/**
 * This filter when applied to bmp image blurs the image.
 * @param data: the threadData struct with information.
 */
void* box_blur_filter(void *data) {

    struct threadData* info = (struct threadData*)data;
    Image *img = info->inImage;
    Image *outputImg = info->outImage;

    for(int i = 1; i < img->height - 1; i++) {
        for(int j = 1; j < img->width - 1; j++) {
            int sumRed = 0;
            int sumBlue = 0;
            int sumGreen = 0;
            int count = 0;

            for(int k = -1; k <= 1; k++) {
                for(int l = -1; l <= 1; l++) {
                    struct Pixel p = img->pArr[i +k][j + l];
                    sumRed += p.red;
                    sumGreen += p.green;
                    sumBlue += p.blue;
                    count++;
                }

            }
            outputImg->pArr[i-1][j-1].red = sumRed / count;
            outputImg->pArr[i-1][j-1].blue = sumBlue / count;
            outputImg->pArr[i-1][j-1].green = sumGreen / count;
        }
    }

    pthread_exit(0);
}
/**
 * This filter applies yellow tint and then randomly draws black circles in an sub image. 25% are small,
 * 25% are large, and 50% are average sized holes.
 * @param data: the threadData struct with information.
 */
void *applySwissCheeseFilter(void* data) {
    struct threadData* info = (struct threadData*)data;
    int startCol = info->startCol;
    int endCol = info->endCol;
    int height = info->height;
    int subregionWidth = endCol - startCol;
    Image* output = info->outImage;
    Image* subImage = info->inImage;

    // Apply yellow tint to the entire sub-region first
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < subregionWidth; j++) {
            output->pArr[i][j].red = ((subImage->pArr[i][j].red * 1.2) > 255 ? 255 : (int)(subImage->pArr[i][j].red * 1.2));
            output->pArr[i][j].blue = ((subImage->pArr[i][j].blue * 0.6) < 0 ? 0 : (int)(subImage->pArr[i][j].blue * 0.6));
            output->pArr[i][j].green = ((subImage->pArr[i][j].green * 1.1) > 255 ? 255 : (int)(subImage->pArr[i][j].green * 1.1));
        }
    }

    //Draw black circle son sub image
    for (int h = 0; h < info->numHoles; h++) {
        struct hole currentHole = holes[h];

        int circleLeft = currentHole.xorigin - currentHole.radius;
        int circleRight = currentHole.xorigin + currentHole.radius;

        if (circleRight >= startCol && circleLeft < endCol) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < subregionWidth; j++) {
                    int globalCol = j + startCol;
                    int dx = globalCol - currentHole.xorigin;
                    int dy = i - currentHole.yorigin;
                    if (dx * dx + dy * dy <= currentHole.radius * currentHole.radius) {
                        output->pArr[i][j].red = 0;
                        output->pArr[i][j].blue = 0;
                        output->pArr[i][j].green = 0;
                    }
                }
            }
        }
    }

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    char *outputFile = NULL;
    char *inputFile = NULL;
    struct BMP_Header BMP;
    struct DIB_Header DIB;
    struct Image* img = NULL;
    struct Pixel** pixels = NULL;
    int boxBlur = 0;
    int swissCheese = 0;
    int numHoles = 0;


    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc && (i == 1)) {
            inputFile = argv[++i];
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
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputFile = argv[++i];
        }
        else if(strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            char* flag = argv[++i];
            if(strcmp(flag, "b") == 0)
                boxBlur = 1;
            else if(strcmp(flag, "c") == 0) {
                swissCheese = 1;
                srand(time(NULL));
                numHoles = computeHoles(img->height, img->width);
            }
        }

    }


    if(img != NULL && pixels != NULL) {

        //collection of threads
        pthread_t tids[THREAD_COUNT];
        //double pointer to threadData for the different threads
        struct threadData** threadData = (struct threadData**)malloc(sizeof(struct threadData*) * THREAD_COUNT);
        //Number of colums from the picture that a thread will work on
        int colsPerThread = img->width / THREAD_COUNT;

        //pointer to final output image
        Image* finalOutputImage;
        // Allocate pixel array for threadOutputImages[t]
        struct Pixel** finalNewpix = (struct Pixel**)malloc(sizeof(struct Pixel*) * img->height);
        for (int i = 0; i < img->height; i++) {
            finalNewpix[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * img->width);
        }
        finalOutputImage = image_create(finalNewpix, img->width, img->height);

        //Create threads to process different parts of the image.
        for(int t = 0; t < THREAD_COUNT; t++) {
            threadData[t] = (struct threadData*)malloc(sizeof(struct threadData));
            int startCol = t * colsPerThread;
            int endCol = (t == THREAD_COUNT - 1) ? img->width : (t+1) * colsPerThread;

            threadData[t]->startCol = startCol;
            threadData[t]->endCol = endCol;
            threadData[t]->width = img->width;
            threadData[t]->height = img->height;
            threadData[t]->outImage = allocate_thread_out_image(img->height, endCol - startCol);
            //boxBlur filter thread
            if(boxBlur == 1) {

                threadData[t]->inImage = allocate_padded_sub_image(img, startCol, endCol);
                pthread_create(&tids[t], NULL, box_blur_filter, threadData[t]);
            }
            //swiss cheese filter thread
            if(swissCheese == 1) {
                threadData[t]->numHoles = numHoles;
                threadData[t]->inImage = allocate_sub_image(img, startCol, endCol);
                pthread_create(&tids[t], NULL, applySwissCheeseFilter, threadData[t]);
            }
        }
        //Wait for all threads to complete
        for(int t = 0; t < THREAD_COUNT; t++) {
            pthread_join(tids[t], NULL);
        }

        for (int t = 0; t < THREAD_COUNT; t++) {
            int startCol = threadData[t]->startCol;
            int endCol = threadData[t]->endCol;
            Image* threadOutImage = threadData[t]->outImage;

            // Merge thread-specific outImage into the final output image
            for (int i = 0; i < img->height; i++) {
                for (int j = 0; j < (endCol - startCol); j++) {
                    finalOutputImage->pArr[i][startCol + j] = threadOutImage->pArr[i][j];
                }
            }

            // Free the thread-specific outImage
            image_destroy(&(threadData[t]->outImage));
        }


        if (outputFile) {
            FILE* file_output = fopen(outputFile, "wb");
            if (!file_output) {
                fprintf(stderr, "Error: Could not open output file %s.\n", outputFile);
                return 1;
            }

            struct BMP_Header newBMP;
            struct DIB_Header newDIB;
            makeBMPHeader(&newBMP, image_get_width(finalOutputImage), image_get_height(finalOutputImage));
            writeBMPHeader(file_output, &newBMP);
            makeDIBHeader(&newDIB, image_get_width(finalOutputImage), image_get_height(finalOutputImage));
            writeDIBHeader(file_output, &newDIB);
            writePixelsBMP(file_output, image_get_pixels(finalOutputImage), image_get_width(finalOutputImage),
            image_get_height(finalOutputImage));
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
            writePixelsBMP(file_output, image_get_pixels(finalOutputImage), image_get_width(finalOutputImage),
            image_get_height(finalOutputImage));
            fclose(file_output);
        }

        //clean up below...
        //freed img
        image_destroy(&img);
        //free finalOutputImage
        image_destroy(&finalOutputImage);

        //free the threadData
        for(int t = 0; t < THREAD_COUNT; t++) {
            free(threadData[t]);
        }
        free(threadData);

    }

    return 0;
}
