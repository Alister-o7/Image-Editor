#ifndef GRAYSCALECOMMAND_H
#define GRAYSCALECOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class grayScaleCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void grayScale();
public:
    explicit grayScaleCommand(Image &image);
    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // GRAYSCALECOMMAND_H
