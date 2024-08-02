#ifndef FLIPXCOMMAND_H
#define FLIPXCOMMAND_H

#include "../Image.h"
#include "ICommand.h"

class flipXCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void flipX();
public:
    explicit flipXCommand(Image &image);

    void execute() override;
    void undo() override;
    void redo() override;
};

#endif // FLIPXCOMMAND_H
