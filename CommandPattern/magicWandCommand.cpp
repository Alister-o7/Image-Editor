#include "magicWandCommand.h"
#include <stack>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace std {
    template <>
    struct hash<QPointF> {
        size_t operator()(const QPointF& p) const {
            return hash<double>()(p.x()) ^ hash<double>()(p.y());
        }
    };
}

    magicWandCommand::magicWandCommand(Image &image, QPointF &pos, std::vector<QPointF> &posList, bool isPremultiplied, int tolerance) : image(image), pos(pos), posList(posList), isPremultiplied(isPremultiplied), tolerance(tolerance), pixelBuffer(image.getPixelBuffer()) {
    backupPixelBuffer = pixelBuffer;
    backupPosList = posList;
    currentPosList = posList;
}

void magicWandCommand::execute() {
    magicWand();
}

void magicWandCommand::undo() {
    posList = backupPosList;
}

void magicWandCommand::redo() {
    posList = currentPosList;
}

void magicWandCommand::floodFill(int x, int y, std::vector<Pixel> &tempPixelBuffer, const Pixel &oldColor, const QColor &newColor, std::vector<QPointF> &tempPosList)
{
    int w = image.getW();
    int h = image.getH();

    qDebug("Command Tolerance: %d", tolerance);

    std::stack<std::pair<int, int>> pixelStack;
    pixelStack.push({x, y});

    while (!pixelStack.empty()) {
        auto [cx, cy] = pixelStack.top();
        pixelStack.pop();

        if (cx < 0 || cx >= w || cy < 0 || cy >= h) {
            continue;
        }

        Pixel &currentPixel = tempPixelBuffer[cy * w + cx];

        if (!currentPixel.compareThreshold(oldColor, tolerance, isPremultiplied)) {
            if (currentPixel != Pixel::fromQColor(newColor)) {
                tempPosList.push_back(QPointF(cx, cy));
            }
            continue;
        }

        currentPixel.setPixel(newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());

        pixelStack.push({cx + 1, cy});
        pixelStack.push({cx - 1, cy});
        pixelStack.push({cx, cy + 1});
        pixelStack.push({cx, cy - 1});
    }
}

static double calculateAngle(const QPointF &origin, const QPointF point)
{
    double minY = point.y() - origin.y();
    double minX = point.x() - origin.x();

    return atan2(minY, minX);
}

static double distanceTo(const QPointF &first, const QPointF &second)
{
    return std::sqrt(std::pow(first.x() - second.x(), 2) + std::pow(first.y() - second.y(), 2));
}

static QPointF findClosestToOrigin(const std::vector<QPointF>& points)
{
    QPointF closest = points[0];
    double minDistance = closest.manhattanLength();

    for (const auto &point : points) {
        double distance = point.manhattanLength();
        if (distance < minDistance) {
            minDistance = distance;
            closest = point;
        }
    }

    return closest;
}

static std::vector<QPointF> sortPointsInOrder(const std::vector<QPointF> &points)
{
    std::vector<QPointF> sortedPoints = points;
    QPointF origin(0, 0);

    std::sort(sortedPoints.begin(), sortedPoints.end(), [&origin](const QPointF& a, const QPointF& b) {
        return calculateAngle(origin, a) < calculateAngle(origin, b);
    });

    QPointF startPoint = findClosestToOrigin(sortedPoints);

    std::vector<QPointF> result;
    result.push_back(startPoint);

    std::unordered_set<QPointF> visited;
    visited.insert(startPoint);

    QPointF currentPoint = startPoint;
    while (result.size() < points.size()) {
        auto closestIter = sortedPoints.end();
        double minDistance = std::numeric_limits<double>::max();
        for (auto iter = sortedPoints.begin(); iter != sortedPoints.end(); ++iter) {
            if (visited.find(*iter) == visited.end()) {
                double distance = distanceTo(currentPoint, *iter);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestIter = iter;
                }
            }
        }
        currentPoint = *closestIter;
        result.push_back(currentPoint);
        visited.insert(currentPoint);
    }

    return result;
}

static std::vector<QPointF> popCutOffPoint(const std::vector<QPointF> &points)
{
    std::vector<QPointF> result;
    QPointF cutOff(5, 5);
    //QPointF average(0, 0);
    //QPointF origin(0, 0);

    for (const auto &point : points) {
        if (point.x() <= cutOff.x() || point.y() <= cutOff.y()) {
            continue;
        }
        result.push_back(point);
    }

    return result;
}

void magicWandCommand::magicWand()
{
    currentPosList.clear();

    std::vector<Pixel> tempPixelBuffer = pixelBuffer;
    std::vector<QPointF> tempPosList;
    QColor newColor(255, 255, 255, 0);
    int posX = int(pos.x());
    int posY = int(pos.y());
    Pixel oldColor = tempPixelBuffer[posY * image.getW() + posX];

    if (oldColor == Pixel::fromQColor(newColor)) {
        return;
    }

    floodFill(posX, posY, tempPixelBuffer, oldColor, newColor, tempPosList);

    qDebug("tempPosList Size: %zu", tempPosList.size());

    if (!tempPosList.empty()) {
        tempPosList = sortPointsInOrder(tempPosList);
        tempPosList = popCutOffPoint(tempPosList);

        posList = tempPosList;

        currentPosList.reserve(tempPosList.size());

        for (auto const &position : tempPosList)
            currentPosList.push_back(position);

    } else {
        posList = tempPosList;
        currentPosList.reserve(0);
    }
}
