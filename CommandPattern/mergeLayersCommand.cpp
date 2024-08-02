#include "mergeLayersCommand.h"

mergeLayersCommand::mergeLayersCommand(Image &image, Image &qImage, int px, int py) : image(image), qImage(qImage), px(px), py(py), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
    qImagePixelBuffer = qImage.getPixelBuffer();
    qImageBackupPixelBuffer = qImagePixelBuffer;
    bufferPx = px;
    bufferPy = py;
    qImageWidth = qImage.getW();
    qImageHeight = qImage.getH();
    qImageBackupWidth = qImageWidth;
    qImageBackupHeight = qImageHeight;
}

void mergeLayersCommand::execute() {
    mergeLayers();
}

void mergeLayersCommand::undo() {
    pixelBuffer = backupPixelBuffer;
    qImagePixelBuffer = qImageBackupPixelBuffer;
    px = bufferPx;
    py = bufferPy;
    qImageWidth = qImageBackupWidth;
    qImageHeight = qImageBackupHeight;
}

void mergeLayersCommand::redo() {
    execute();
}

void mergeLayersCommand::mergeLayers() {
    std::vector<Pixel> tempPixelBuffer = pixelBuffer;

    for (int i = 0; i < qImageHeight; i++) {
        for (int j = 0; j < qImageWidth; j++) {
            tempPixelBuffer[(i+py) * image.getW() + (j+px)] = qImagePixelBuffer[i * qImageWidth + j];
        }
    }

    pixelBuffer = tempPixelBuffer;
}
