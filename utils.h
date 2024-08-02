#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "Pixel.h"
#include "Image.h"

void applyKernel(double kernel[3][3], Image& image, std::vector<Pixel>& pixelBuffer);
void applyCustomKernel(const std::vector<std::vector<double>>& kernel, Image& image, std::vector<Pixel>& pixelBuffer);

int truncate0_255(int value);
int truncate_m100_100(int value);
int truncate_0_100(int value);

#endif // UTILS_H
