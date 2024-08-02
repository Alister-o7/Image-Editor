#ifndef MAGICWANDCOMMAND_H
#define MAGICWANDCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class magicWandCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    std::vector<QPointF> &posList;
    std::vector<QPointF> backupPosList;
    std::vector<QPointF> currentPosList;
    QPointF &pos;
    bool isPremultiplied;
    int tolerance;

    void floodFill(int x, int y, std::vector<Pixel> &tempPixelBuffer, const Pixel &oldColor, const QColor &newColor, std::vector<QPointF> &tempPosList);
    void magicWand();

public:
    explicit magicWandCommand(Image &image, QPointF &pos, std::vector<QPointF> &posList, bool isPremultiplied, int tolerance);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // MAGICWANDCOMMAND_H
