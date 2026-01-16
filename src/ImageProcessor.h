#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>

class ImageProcessor {
public:
    static QImage adjustBrightness(const QImage& image, int value);
    static QImage adjustContrast(const QImage& image, int value);
    static QImage adjustSaturation(const QImage& image, int value);
    static QImage adjustHue(const QImage& image, int value);
    static QImage applyAdjustments(const QImage& image, int brightness, int contrast, int saturation, int hue);

private:
    ImageProcessor() = default;
};

#endif
