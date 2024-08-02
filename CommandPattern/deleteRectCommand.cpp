#include "deleteRectCommand.h"

deleteRectCommand::deleteRectCommand(Image &image, int rectPosX, int rectPosY, int rectW, int rectH) : image(image), rectPosX(rectPosX), rectPosY(rectPosY), rectW(rectW), rectH(rectH), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
}

void deleteRectCommand::execute() {
    deleteRect();
}

void deleteRectCommand::undo() {
    pixelBuffer = backupPixelBuffer;
}

void deleteRectCommand::redo() {
    execute();
}

void deleteRectCommand::deleteRect()
{
    std::vector<Pixel> tempPixelBuffer = pixelBuffer;
    for (int i = rectPosY; i < (rectH+rectPosY); i++) {
        for (int j = rectPosX; j < (rectW+rectPosX); j++) {
            tempPixelBuffer[i * image.getW() + j].setPixel(255,255,255,0);
        }
    }

    pixelBuffer = tempPixelBuffer;
}
