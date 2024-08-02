#ifndef DELETERECTCOMMAND_H
#define DELETERECTCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class deleteRectCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    int rectPosX;
    int rectPosY;
    int rectW;
    int rectH;

    void deleteRect();

public:
    deleteRectCommand(Image &image, int rectPosX, int rectPosY, int rectW, int rectH);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // DELETERECTCOMMAND_H
