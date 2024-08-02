#ifndef EMBOSSCOMMAND_H
#define EMBOSSCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class embossCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void emboss();

public:
    explicit embossCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;


};

#endif // EMBOSSCOMMAND_H
