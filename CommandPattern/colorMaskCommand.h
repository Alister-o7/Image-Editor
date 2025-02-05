#ifndef COLORMASKCOMMAND_H
#define COLORMASKCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class colorMaskCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    int r, g, b;

    void colorMask();
public:
    explicit colorMaskCommand(Image &, int r, int g, int b);
    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // COLORMASKCOMMAND_H
