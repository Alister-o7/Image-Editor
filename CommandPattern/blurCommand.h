#ifndef BLURCOMMAND_H
#define BLURCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class blurCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void blur();

public:
    explicit blurCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // BLURCOMMAND_H
