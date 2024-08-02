#include "autoCropCommand.h"
#include <cmath>

autoCropCommand::autoCropCommand(Image &image, int is_black) : image(image), is_black(is_black), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
}

void autoCropCommand::execute() {
    autoCrop();
}

void autoCropCommand::undo() {
    pixelBuffer = backupPixelBuffer;
    image.setW(backupW);
    image.setH(backupH);
}

void autoCropCommand::redo() {
    execute();
}

void autoCropCommand::autoCrop() {
    Pixel temp;
    Pixel compare;
    int x_min = image.getW();
    int y_min = image.getH();
    int x_max = 0;
    int y_max = 0;

    if (is_black == 0) {
        compare = Pixel(0,0,0,255);
    } else {
        compare = Pixel(255,255,255,255);
    }

    //qDebug("compare r,g,b: %d, %d, %d", compare.getR(), compare.getG(), compare.getB());

    for (int y = 0; y < image.getH(); y++) {
        for (int x = 0; x < image.getW(); x++) {
            temp = pixelBuffer[x + y * image.getW()];

            if (temp.getR() != compare.getR() && temp.getG() != compare.getG() && temp.getB() != compare.getB()) {
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
                if (y < y_min) y_min = y;
                if (y > y_max) y_max = y;
            }
        }
    }

    int newW = abs(x_max - x_min);
    int newH = abs(y_max - y_min);

    std::vector<Pixel> newPixelBuffer;
    newPixelBuffer.resize(newW*newH);

    //qDebug("y_min, x_min: %d, %d", y_min, x_min);

    for (int y = 0; y < newH; y++) {
        for (int x = 0; x < newW; x++) {
            newPixelBuffer[y * newW + x] = pixelBuffer[(y_min+y) * image.getW() + (x_min+x)];
        }
    }

    backupW = image.getW();
    image.setW(newW);

    backupH = image.getH();
    image.setH(newH);

    pixelBuffer = newPixelBuffer;
}
