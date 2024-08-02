#include "colorSaturationCommand.h"

colorSaturationCommand::colorSaturationCommand(Image &image, int r, int g, int b) :
    image(image), r(r), g(g), b(b), pixelBuffer(image.getPixelBuffer()){
    backupPixelBuffer = pixelBuffer;
}

void colorSaturationCommand::execute() {
    colorSaturation();
}

void colorSaturationCommand::undo() {
    pixelBuffer = backupPixelBuffer;
}

void colorSaturationCommand::redo() {
    execute();
}

void colorSaturationCommand::colorSaturation()
{
    for (int i = 0; i < image.getSize(); i++) {
        pixelBuffer[i].setPixel(pixelBuffer[i].getR() + r,
                                pixelBuffer[i].getG() + g,
                                pixelBuffer[i].getB() + b,
                                pixelBuffer[i].getA());
    }
}
