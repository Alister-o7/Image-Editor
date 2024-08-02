#include "blurGaussianCommand.h"
#include "../utils.h"
#include <algorithm>
#include <cmath>

blurGaussianCommand::blurGaussianCommand(Image &image, int radius) : image(image), radius(radius), pixelBuffer(image.getPixelBuffer()) {
    previousPixelBuffer = pixelBuffer;
    nextPixelBuffer = pixelBuffer;
}

void blurGaussianCommand::execute() {
    blurGaussian();
}

void blurGaussianCommand::undo() {
    pixelBuffer = previousPixelBuffer;
}

void blurGaussianCommand::redo() {
    pixelBuffer = nextPixelBuffer;
}

void blurGaussianCommand::blurGaussian()
{
    double sigma = std::max(double(radius / 2), double(1));
    int kernelWidth = (2 * radius) + 1;
    std::vector<std::vector<double>> blurKer;
    blurKer.resize(kernelWidth);
    for (int y = 0; y < kernelWidth; y++) {
        blurKer[y].resize(kernelWidth);
    }
    double sum = 0;

    for (int x = -radius; x < radius; x++) {
        for (int y = -radius; y < radius; y++) {
            double exponentNumerator = double(-(x * x + y * y));
            double exponentDenominator = (double(2) * sigma * sigma);

            double eExpression = std::pow(euler, exponentNumerator / exponentDenominator);
            double kernelValue = (eExpression / (2 * pi * (sigma * sigma)));

            blurKer[x+radius][y+radius] = kernelValue;
            sum += kernelValue;
        }
    }

    // Normalize kernel
    for (int x = 0; x < kernelWidth; x++) {
        for (int y = 0; y < kernelWidth; y++) {
            blurKer[x][y] /= sum;
        }
    }

    applyCustomKernel(blurKer, image, pixelBuffer);

    blurKer.clear();

    blurKer.shrink_to_fit();

    nextPixelBuffer = pixelBuffer;
}
