#ifndef PIXMAPTOIMAGECOMMAND_H
#define PIXMAPTOIMAGECOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class pixmapToImageCommand : public ICommand {
private:
    Image &image;
    Image &qImage;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    std::vector<Pixel> qImagePixelBuffer;
    std::vector<Pixel> qImageBackupPixelBuffer;
    bool isEraser;

    QPointF calculateCentroid(const std::vector<QPointF> &points);
    double calculateAngle(const QPointF &centroid, const QPointF point);
    void sortPointsClockwise(std::vector<QPointF> &points);
    void pixmapToImage();
public:
    explicit pixmapToImageCommand(Image &image, Image &qImage, bool isEraser);

    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // PIXMAPTOIMAGECOMMAND_H
