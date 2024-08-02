#ifndef MERGELAYERSCOMMAND_H
#define MERGELAYERSCOMMAND_H

#include "ICommand.h"
#include "../Image.h"

class mergeLayersCommand : public ICommand {
private:
    Image &image;
    Image &qImage;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> backupPixelBuffer;
    std::vector<Pixel> qImagePixelBuffer;
    std::vector<Pixel> qImageBackupPixelBuffer;
    int px;
    int py;
    int bufferPx;
    int bufferPy;
    int qImageWidth;
    int qImageHeight;
    int qImageBackupWidth;
    int qImageBackupHeight;

    void mergeLayers();

public:
    mergeLayersCommand(Image &image, Image &qImage, int px, int py);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // MERGELAYERSCOMMAND_H
