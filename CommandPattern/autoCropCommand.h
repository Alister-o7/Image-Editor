#ifndef AUTOCROPCOMMAND_H
#define AUTOCROPCOMMAND_H

#include "../Image.h"
#include "ICommand.h"

class autoCropCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    int backupW;
    int backupH;

    int is_black;

    void autoCrop();
public:
    explicit autoCropCommand(Image &image, int is_black);

    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // AUTOCROPCOMMAND_H
