#ifndef SATURATIONCOMMAND_H
#define SATURATIONCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class saturationCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    int r, g, b;

    void saturation();
public:
    explicit saturationCommand(Image &image, int r, int g, int b);
    void execute() override();
    void undo() override();
    void redo() override();
};

#endif // SATURATIONCOMMAND_H
