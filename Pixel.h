#ifndef PIXEL_H
#define PIXEL_H

#include <cstdint>
#include <QColor>

class Pixel {
private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
public:
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    Pixel(); // all black
    Pixel(const Pixel &pixel);

    bool operator==(const Pixel &pixel) const;
    bool operator!=(const Pixel &pixel) const;
    bool compareThreshold(const Pixel &pixel, int threshold, bool isPremultiplied) const;

    uint8_t getR() const;
    uint8_t getG() const;
    uint8_t getB() const;
    uint8_t getA() const;

    void setPixel(int r, int g, int b, int a);

    static QColor toQColor(const Pixel &);
    static Pixel fromQColor(const QColor &);
};

#endif // PIXEL_H
