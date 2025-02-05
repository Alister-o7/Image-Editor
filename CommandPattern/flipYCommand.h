#ifndef FLIPYCOMMAND_H
#define FLIPYCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class flipYCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void flipY();

public:
    explicit flipYCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // FLIPYCOMMAND_H
