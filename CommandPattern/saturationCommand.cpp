#include "saturationCommand.h"

saturationCommand::saturationCommand(Image &image, int r, int g, int b) :
    image(image), r(r), g(g), b(b), pixelBuffer(image.getPixelBuffer()){
    backupPixelBuffer = pixelBuffer;
}

void saturationCommand::execute() {
    colorMask();
}

void saturationCommand::undo() {
    pixelBuffer = backupPixelBuffer;
}

void saturationCommand::redo() {
    execute();
}

void saturationCommand::saturation() {
    int rRatio, gRatio, bRatio;

    rRatio = (r / 100) + 1;
    gRatio = (g / 100) + 1;
    bRatio = (b / 100) + 1;


}
