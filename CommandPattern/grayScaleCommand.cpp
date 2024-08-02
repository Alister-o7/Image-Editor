#include "grayScaleCommand.h"

grayScaleCommand::grayScaleCommand(Image &image) : image(image), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
}

void grayScaleCommand::execute() {
    grayScale();
}

void grayScaleCommand::undo() {
    pixelBuffer = backupPixelBuffer;
}

void grayScaleCommand::redo() {
    execute();
}

void grayScaleCommand::grayScale() {
    for (int y = 0; y < image.getH(); y++) {
        for(int x = 0; x < image.getW(); x++) {
            int gray = 0.2126 * pixelBuffer[y * image.getW() + x].getR() + 0.7125 * pixelBuffer[y * image.getW() + x].getG() + 0.0722 * pixelBuffer[y * image.getW() + x].getB();
            pixelBuffer[y * image.getW() + x].setPixel(gray, gray, gray, pixelBuffer[y * image.getW() + x].getA());
        }
    }
}
