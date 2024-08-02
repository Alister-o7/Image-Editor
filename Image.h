#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QString>
#include "Pixel.h"
#include <vector>

using namespace std;

class Image {
private:
    QImage rawImage;
    QString path;
    QString filename;
    int w;
    int h;
    std::vector<Pixel> pixelBuffer;
    size_t size{0};
    bool isImageValid;
    void initPixelBuffer();
    void pureFilename();
public:
    explicit Image(const QString &path);

    explicit Image(const QImage &qimage);

    bool save(const QString &outPath, int quality = -1); // -1 auto compression // 0-100 quality range
    Image(Image &Image) = delete;

    QString getPath() const;
    QString getFilename() const;

    std::vector<Pixel>& getPixelBuffer();

    size_t getSize() const;

    int getW() const;
    int getH() const;

    void setW(int w);
    void setH(int h);

    void setPath(QString path);

    void swapDimension();

    void updateBuffer();

    QImage& getQImage();

    bool isValid() const;
};

#endif // IMAGE_H
