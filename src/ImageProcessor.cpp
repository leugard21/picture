#include "ImageProcessor.h"

#include <algorithm>
#include <cmath>

QImage ImageProcessor::adjustBrightness(const QImage &image, int value) {
  if (image.isNull() || value == 0) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      int r = qRed(line[x]) + value;
      int g = qGreen(line[x]) + value;
      int b = qBlue(line[x]) + value;
      int a = qAlpha(line[x]);

      r = std::clamp(r, 0, 255);
      g = std::clamp(g, 0, 255);
      b = std::clamp(b, 0, 255);

      line[x] = qRgba(r, g, b, a);
    }
  }

  return result;
}

QImage ImageProcessor::adjustContrast(const QImage &image, int value) {
  if (image.isNull() || value == 0) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  double factor = (259.0 * (value + 255.0)) / (255.0 * (259.0 - value));

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      int r = static_cast<int>(factor * (qRed(line[x]) - 128) + 128);
      int g = static_cast<int>(factor * (qGreen(line[x]) - 128) + 128);
      int b = static_cast<int>(factor * (qBlue(line[x]) - 128) + 128);
      int a = qAlpha(line[x]);

      r = std::clamp(r, 0, 255);
      g = std::clamp(g, 0, 255);
      b = std::clamp(b, 0, 255);

      line[x] = qRgba(r, g, b, a);
    }
  }

  return result;
}

QImage ImageProcessor::adjustSaturation(const QImage &image, int value) {
  if (image.isNull() || value == 0) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  double factor = 1.0 + (value / 100.0);

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      int r = qRed(line[x]);
      int g = qGreen(line[x]);
      int b = qBlue(line[x]);
      int a = qAlpha(line[x]);

      double gray = 0.299 * r + 0.587 * g + 0.114 * b;

      r = static_cast<int>(gray + factor * (r - gray));
      g = static_cast<int>(gray + factor * (g - gray));
      b = static_cast<int>(gray + factor * (b - gray));

      r = std::clamp(r, 0, 255);
      g = std::clamp(g, 0, 255);
      b = std::clamp(b, 0, 255);

      line[x] = qRgba(r, g, b, a);
    }
  }

  return result;
}

QImage ImageProcessor::adjustHue(const QImage &image, int value) {
  if (image.isNull() || value == 0) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      QColor color = QColor::fromRgba(line[x]);
      int h, s, l, a;
      color.getHsl(&h, &s, &l, &a);

      if (h >= 0) {
        h = (h + value) % 360;
        if (h < 0)
          h += 360;
      }

      color.setHsl(h, s, l, a);
      line[x] = color.rgba();
    }
  }

  return result;
}

QImage ImageProcessor::applyAdjustments(const QImage &image, int brightness,
                                        int contrast, int saturation, int hue) {
  if (image.isNull()) {
    return image;
  }

  QImage result = image;

  if (brightness != 0) {
    result = adjustBrightness(result, brightness);
  }
  if (contrast != 0) {
    result = adjustContrast(result, contrast);
  }
  if (saturation != 0) {
    result = adjustSaturation(result, saturation);
  }
  if (hue != 0) {
    result = adjustHue(result, hue);
  }

  return result;
}
