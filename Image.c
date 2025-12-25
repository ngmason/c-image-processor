/**
* An image ADT implementation.
*
* Completion time: 300 minutes
*
* @author Nina Mason
* @version 10/29/24
*/
#include "Image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Creates a new image and returns it.
*
* @param pArr: Pixel array of this image.
* @param width: Width of this image.
* @param height: Height of this image.
* @return A pointer to a new image.
*/
Image* image_create(struct Pixel** pArr, int width, int height) {
    Image* img = (struct Image*)malloc(sizeof(Image));

    img->width = width;
    img->height = height;
    img->pArr = pArr;
    return img;
}
/* Destroys an image. Does not deallocate internal pixel array.
*
* @param img: the image to destroy.
*/
void image_destroy(Image** img) {
    if (img && *img) {
        free(*img);
        *img = NULL;
    }
}
/* Returns a double pointer to the pixel array.
*
* @param img: the image.
*/
struct Pixel** image_get_pixels(Image* img) {
    return img->pArr;
}
/* Returns the width of the image.
*
* @param img: the image.
*/
int image_get_width(Image* img) {
    return img->width;
}
/* Returns the height of the image.
*
* @param img: the image.
*/
int image_get_height(Image* img) {
    return img->height;
}
/* Converts the image to grayscale.
*
* @param img: the image.
*/
void image_apply_bw(Image* img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            struct Pixel pixel = img->pArr[i][j];
            int gray = (int)(0.299*img->pArr[i][j].red + 0.587*img->pArr[i][j].green + 0.114*img->pArr[i][j].blue);
            img->pArr[i][j].red = gray;
            img->pArr[i][j].green = gray;
            img->pArr[i][j].blue = gray;
        }
    }
}
/**
* Shift color of the internal Pixel array. The dimension of the array is width * height.
* The shift value of RGB is rShift, gShift，bShift. Useful for color shift.
*
* @param img: the image.
* @param rShift: the shift value of color r shift
* @param gShift: the shift value of color g shift
* @param bShift: the shift value of color b shift
*/
void image_apply_colorshift(Image* img, int rShift, int gShift, int bShift) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            struct Pixel pixel = img->pArr[i][j];
            int red = pixel.red + rShift;
            int green = pixel.green + gShift;
            int blue = pixel.blue + bShift;
            img->pArr[i][j].red = (red > 255? 255 : (red < 0 ? 0 : red));
            img->pArr[i][j].green = (green > 255? 255 : (green < 0 ? 0 : green));
            img->pArr[i][j].blue = (blue > 255? 255 : (blue < 0 ? 0 : blue));
        }
    }
}
/* Resizes the image. If the scaling factor is less than 1 the new image will be
* smaller, if it is larger than 1, the new image will be larger.
*
* @param img: the image.
* @param factor: the scaling factor
*/
void image_apply_resize(Image* img, float factor) {

    Image *tmp = (Image*)malloc(sizeof(Image));
    tmp->width = (int)(img->width * factor);
    tmp->height = (int)(img->height * factor);

    // Allocate new array for resized image
    tmp->pArr = (struct Pixel**)malloc(sizeof(struct Pixel*) * tmp->height);

    for (int i = 0; i < tmp->height; i++) {
        tmp->pArr[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * tmp->width);
    }

    // Fill the new image array with nearest neighbor pixels from the original image
    for (int i = 0; i < tmp->height; i++) {
        for (int j = 0; j < tmp->width; j++) {
            tmp->pArr[i][j] = img->pArr[(int)(i/factor)][(int)(j/factor)];
        }
    }

    // Free the old pixel array
    for (int i = 0; i < img->height; i++) {
        free(img->pArr[i]);
    }
    free(img->pArr);

    // Update image with new dimensions and pixel array
    img->width = tmp->width;
    img->height = tmp->height;
    img->pArr = tmp->pArr;
    free(tmp);

}