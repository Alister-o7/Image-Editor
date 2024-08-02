#include "paintBucketCommand.h"
#include <stack>

paintBucketCommand::paintBucketCommand(Image &image, QColor &newColor, QPointF &pos) : image(image), newColor(newColor), pos(pos), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
}

void paintBucketCommand::execute() {
    paintBucket();
}

void paintBucketCommand::undo() {
    pixelBuffer = backupPixelBuffer;
}

void paintBucketCommand::redo() {
    execute();
}

void paintBucketCommand::dfs(int x, int y, std::vector<Pixel> &tempPixelBuffer, const Pixel &oldColor) {
    int w = image.getW();
    int h = image.getH();

    std::stack<std::pair<int, int>> pixelStack;
    pixelStack.push({x, y});

    while (!pixelStack.empty()) {
        auto [cx, cy] = pixelStack.top();
        pixelStack.pop();

        if (cx < 0 || cx >= w || cy < 0 || cy >= h) {
            continue;
        }

        Pixel &currentPixel = tempPixelBuffer[cy * w + cx];

        if (currentPixel != oldColor) {
            continue;
        }

        currentPixel.setPixel(newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());

        pixelStack.push({cx + 1, cy});
        pixelStack.push({cx - 1, cy});
        pixelStack.push({cx, cy + 1});
        pixelStack.push({cx, cy - 1});
    }
}

void paintBucketCommand::paintBucket()
{
    std::vector<Pixel> tempPixelBuffer = pixelBuffer;
    int posX = int(pos.x());
    int posY = int(pos.y());
    Pixel oldColor = tempPixelBuffer[posY*image.getW() + posX];

    if (oldColor == Pixel::fromQColor(newColor)) {
        return;
    }

    dfs(posX, posY, tempPixelBuffer, oldColor);

    pixelBuffer = tempPixelBuffer;
}
