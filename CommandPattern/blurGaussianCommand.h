#ifndef BLURGAUSSIANCOMMAND_H
#define BLURGAUSSIANCOMMAND_H

#include "ICommand.h"
#include "../Image.h"
#include <vector>

class blurGaussianCommand : public ICommand {
private:
    Image &image;
    std::vector<Pixel>& pixelBuffer;
    std::vector<Pixel> previousPixelBuffer;
    std::vector<Pixel> nextPixelBuffer;
    const double pi = 3.14159265358979323846;
    const double euler = 2.7182818284590452353602874713527;
    int radius;

    void blurGaussian();

public:
    explicit blurGaussianCommand(Image &image, int radius);

    void execute() override;

    void undo() override;

    void redo() override;
};

#endif // BLURGAUSSIANCOMMAND_H
