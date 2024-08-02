#ifndef COLORSATURATIONCOMMAND_H
#define COLORSATURATIONCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class colorSaturationCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    int r, g, b;

    void colorSaturation();
public:
    explicit colorSaturationCommand(Image &, int r, int g, int b);
    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // COLORSATURATIONCOMMAND_H
