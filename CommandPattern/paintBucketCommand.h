#ifndef PAINTBUCKETCOMMAND_H
#define PAINTBUCKETCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class paintBucketCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    QColor newColor;
    QPointF pos;

    void dfs(int x, int y, std::vector<Pixel> &tempPixelBuffer, const Pixel &oldColor);
    void paintBucket();

public:
    explicit paintBucketCommand(Image &image, QColor &newColor, QPointF &pos);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // PAINTBUCKETCOMMAND_H
