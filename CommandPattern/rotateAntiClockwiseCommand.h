#ifndef ROTATEANTICLOCKWISECOMMAND_H
#define ROTATEANTICLOCKWISECOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class rotateAntiClockwiseCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void rotateAnticlockwise();
public:
    explicit rotateAntiClockwiseCommand(Image &image);

    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // ROTATEANTICLOCKWISECOMMAND_H
