#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>

class ImageProcessor {
public:
  static QImage adjustBrightness(const QImage &image, int value);
  static QImage adjustContrast(const QImage &image, int value);
  static QImage adjustSaturation(const QImage &image, int value);
  static QImage adjustHue(const QImage &image, int value);
  static QImage applyAdjustments(const QImage &image, int brightness,
                                 int contrast, int saturation, int hue);

  static QImage applyGrayscale(const QImage &image);
  static QImage applySepia(const QImage &image);
  static QImage applyInvert(const QImage &image);
  static QImage applyBlur(const QImage &image, int radius = 2);
  static QImage applySharpen(const QImage &image);

private:
  ImageProcessor() = default;
};

#endif
