#include "utils.h"

void applyKernel(double kernel[3][3], Image& image, std::vector<Pixel>& pixelBuffer) {
    double sumR;
    double sumG;
    double sumB;

    int h = image.getH();
    int w = image.getW();

    std::vector<Pixel> tempPixelBuffer;
    tempPixelBuffer.reserve((h+2)*(w+2));
    tempPixelBuffer.resize((h+2)*(w+2));

    for (int i = 1; i < h + 1; i++) {
        for (int j = 1; j < w + 1; j++) {
            tempPixelBuffer[i * (w + 2) + j] = pixelBuffer[(i-1) * w + (j-1)];
        }
    }

    for (int i = 1; i < w + 1; i++) //upper (not round)
        tempPixelBuffer[0 * (w + 2) + i] = pixelBuffer[0 * w + (i - 1)];

    for (int i = 1; i < w + 1; i++) //below
        tempPixelBuffer[(h + 1) * (w + 2) + i] = pixelBuffer[(h - 1) * w + (i - 1)];

    for (int i = 1; i < h + 1; i++) //sx
        tempPixelBuffer[i * (w + 2) + 0] = pixelBuffer[(i - 1) * w + 0];

    for (int i = 1; i < h + 1; i++) //dx
        tempPixelBuffer[i * (w + 2) + (w + 1)] = pixelBuffer[(i - 1) * w + (w - 1)];

    //sx round top
    tempPixelBuffer[0 * (w + 2) + 0] = pixelBuffer[0 * w + 0];

    //dx round top
    tempPixelBuffer[0 * (w + 2) + (w + 1)] = pixelBuffer[0 * w + (w - 1)];

    //sx round below
    tempPixelBuffer[(h + 1) * (w + 2) + 0] = pixelBuffer[(h - 1) * w + 0];

    //dx round below
    tempPixelBuffer[(h + 1) * (w + 2) + (w + 1)] = pixelBuffer[(h - 1) * w + (w - 1)];


    for (int i = 1; i < h + 1; i++) {
        for (int j = 1; j < w + 1; j++) {
            sumR = 0.0;
            sumG = 0.0;
            sumB = 0.0;

            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    sumR += double(tempPixelBuffer[(i + k - 1) * (w + 2) + (j + l - 1)].getR()) * kernel[k][l];

                    sumG += double(tempPixelBuffer[(i + k - 1) * (w + 2) + (j + l - 1)].getG()) * kernel[k][l];

                    sumB += double(tempPixelBuffer[(i + k - 1) * (w + 2) + (j + l - 1)].getB()) * kernel[k][l];
                }
            }

            pixelBuffer[(i - 1) * w + (j - 1)].setPixel(sumR, sumG, sumB, pixelBuffer[(i - 1) * w + (j - 1)].getA());
        }
    }
}

void applyCustomKernel(const std::vector<std::vector<double>>& kernel, Image& image, std::vector<Pixel>& pixelBuffer) {
    double sumR;
    double sumG;
    double sumB;

    int y;
    int x;

    const int h = image.getH();
    const int w = image.getW();

    const int kernelSize = kernel.size();
    const int kCenter = kernelSize / 2;

    const int newWidth = w + 2 * kCenter;

    std::vector<Pixel> tempPixelBuffer;
    tempPixelBuffer.reserve((h+2 * kCenter)*(w+2 * kCenter));
    tempPixelBuffer.resize((h+2 * kCenter)*(w+2 * kCenter));

    for (int i = kCenter; i < h + kCenter; i++) {
        for (int j = kCenter; j < w + kCenter; j++) {
            tempPixelBuffer[i * (newWidth) + j] = pixelBuffer[(i-kCenter) * w + (j-kCenter)];
        }
    }

    for (int i = kCenter; i < w + kCenter; i++) { //upper (not round) && below
        tempPixelBuffer[0 * (newWidth) + i] = pixelBuffer[0 * w + (i - kCenter)];
        tempPixelBuffer[(h + kCenter) * (newWidth) + i] = pixelBuffer[(h - 1) * w + (i - kCenter)];
    }

    for (int i = kCenter; i < h + kCenter; i++) { //sx && dx
        tempPixelBuffer[i * (newWidth) + 0] = pixelBuffer[(i - kCenter) * w + 0];
        tempPixelBuffer[i * (newWidth) + (w + kCenter)] = pixelBuffer[(i - kCenter) * w + (w - 1)];
    }

    //sx round top
    tempPixelBuffer[0 * (newWidth) + 0] = pixelBuffer[0 * w + 0];

    //dx round top
    tempPixelBuffer[0 * (newWidth) + (w + kCenter)] = pixelBuffer[0 * w + (w - kCenter)];

    //sx round below
    tempPixelBuffer[(h + kCenter) * (newWidth) + 0] = pixelBuffer[(h - kCenter) * w + 0];

    //dx round below
    tempPixelBuffer[(h + kCenter) * (newWidth) + (w + kCenter)] = pixelBuffer[(h - kCenter) * w + (w - kCenter)];


    for (int i = kCenter; i < h + kCenter; i++) {
        for (int j = kCenter; j < w + kCenter; j++) {
            sumR = 0.0;
            sumG = 0.0;
            sumB = 0.0;

            for (int k = 0; k < kernelSize; k++) {
                for (int l = 0; l < kernelSize; l++) {
                    y = i + k - kCenter;
                    x = j + l - kCenter;
                    const Pixel& p = tempPixelBuffer[y * (newWidth) + x];
                    sumR += double(p.getR()) * kernel[k][l];

                    sumG += double(p.getG()) * kernel[k][l];;

                    sumB += double(p.getB()) * kernel[k][l];;
                }
            }

            pixelBuffer[(i - kCenter) * w + (j - kCenter)].setPixel(sumR, sumG, sumB, pixelBuffer[(i - kCenter) * w + (j - kCenter)].getA());
        }
    }
}

int truncate0_255(int value) {
    if (value < 0)
        return 0;
    if (value > 255)
        return 255;
    else
        return value;
}

int truncate_m100_100(int value) {
    if (value < -100)
        return -100;
    if (value > 100)
        return 100;
    else
        return value;
}

int truncate0_100(int value) {
    if (value < 0)
        return 0;
    if (value > 100)
        return 100;
    else
        return value;
}
