#include "rotateAntiClockwiseCommand.h"

rotateAntiClockwiseCommand::rotateAntiClockwiseCommand(Image &image) : image(image), pixelBuffer(image.getPixelBuffer()){
    backupPixelBuffer = pixelBuffer;
}
void rotateAntiClockwiseCommand::execute() {
    rotateAnticlockwise();
}

void rotateAntiClockwiseCommand::undo() {
    pixelBuffer = backupPixelBuffer;
    image.swapDimension();
}

void rotateAntiClockwiseCommand::redo() {
    execute();
}

void rotateAntiClockwiseCommand::rotateAnticlockwise() {
    std::vector<Pixel> tempPixelBuffer = pixelBuffer;

    for (int i = 0; i < image.getW(); i++) {
        for (int j = 0; j < image.getH(); j++) {
            pixelBuffer[(image.getW() - i - 1) * image.getH() + j] = tempPixelBuffer[j * image.getW() + i];
        }
    }
    image.swapDimension();
}
