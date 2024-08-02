#include "pixmapToImageCommand.h"

pixmapToImageCommand::pixmapToImageCommand(Image &image, Image &qImage, bool isEraser) : image(image), qImage(qImage), isEraser(isEraser), pixelBuffer(image.getPixelBuffer()){
    backupPixelBuffer = pixelBuffer;
    qImagePixelBuffer = qImage.getPixelBuffer();
    qImageBackupPixelBuffer = qImagePixelBuffer;
}
void pixmapToImageCommand::execute() {
    pixmapToImage();
}

void pixmapToImageCommand::undo() {
    pixelBuffer = backupPixelBuffer;
    qImagePixelBuffer = qImageBackupPixelBuffer;
}

void pixmapToImageCommand::redo() {
    execute();
}

void pixmapToImageCommand::pixmapToImage()
{
    std::vector<Pixel> tempPixelBuffer = pixelBuffer;
    if (isEraser) {
        for (int y = 0; y < image.getH(); y++) {
            for (int x = 0; x < image.getW(); x++) {
                if (tempPixelBuffer[y * image.getW() + x] != qImagePixelBuffer[y * image.getW() + x]) {
                    tempPixelBuffer[y * image.getW() + x].setPixel(255,255,255,0);
                }
            }
        }
    } else {
        for (int y = 0; y < image.getH(); y++) {
            for (int x = 0; x < image.getW(); x++) {
                tempPixelBuffer[y * image.getW() + x] = qImagePixelBuffer[y * image.getW() + x];
            }
        }
    }
    pixelBuffer = tempPixelBuffer;
}
