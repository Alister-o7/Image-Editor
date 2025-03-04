#ifndef ROTATECLOCKWISECOMMAND_H
#define ROTATECLOCKWISECOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class rotateClockwiseCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void rotateClockwise();

public:
    explicit rotateClockwiseCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // ROTATECLOCKWISECOMMAND_H
