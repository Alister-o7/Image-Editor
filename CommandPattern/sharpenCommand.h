#ifndef SHARPENCOMMAND_H
#define SHARPENCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class sharpenCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void sharpen();

public:
    explicit sharpenCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // SHARPENCOMMAND_H
