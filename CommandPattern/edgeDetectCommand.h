#ifndef EDGEDETECTCOMMAND_H
#define EDGEDETECTCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class edgeDetectCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;

    void edgeDetect();

public:
    explicit edgeDetectCommand(Image &image);

    void execute() override;

    void undo() override;

    void redo() override;


};

#endif // EDGEDETECTCOMMAND_H
