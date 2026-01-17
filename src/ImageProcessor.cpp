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

QImage ImageProcessor::applyGrayscale(const QImage &image) {
  if (image.isNull()) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      int gray = qGray(line[x]);
      line[x] = qRgba(gray, gray, gray, qAlpha(line[x]));
    }
  }

  return result;
}

QImage ImageProcessor::applySepia(const QImage &image) {
  if (image.isNull()) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  for (int y = 0; y < result.height(); ++y) {
    QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
    for (int x = 0; x < result.width(); ++x) {
      int r = qRed(line[x]);
      int g = qGreen(line[x]);
      int b = qBlue(line[x]);
      int a = qAlpha(line[x]);

      int tr = static_cast<int>(0.393 * r + 0.769 * g + 0.189 * b);
      int tg = static_cast<int>(0.349 * r + 0.686 * g + 0.168 * b);
      int tb = static_cast<int>(0.272 * r + 0.534 * g + 0.131 * b);

      r = std::min(255, tr);
      g = std::min(255, tg);
      b = std::min(255, tb);

      line[x] = qRgba(r, g, b, a);
    }
  }

  return result;
}

QImage ImageProcessor::applyInvert(const QImage &image) {
  if (image.isNull()) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);
  result.invertPixels(QImage::InvertRgb);
  return result;
}

QImage ImageProcessor::applyBlur(const QImage &image, int radius) {
  if (image.isNull() || radius <= 0) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  QImage temp = result;
  int w = result.width();
  int h = result.height();

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      int r = 0, g = 0, b = 0, a = 0;
      int count = 0;

      for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
          int ny = y + dy;
          int nx = x + dx;

          if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
            QRgb pixel = temp.pixel(nx, ny);
            r += qRed(pixel);
            g += qGreen(pixel);
            b += qBlue(pixel);
            a += qAlpha(pixel);
            count++;
          }
        }
      }

      result.setPixel(x, y, qRgba(r / count, g / count, b / count, a / count));
    }
  }

  return result;
}

QImage ImageProcessor::applySharpen(const QImage &image) {
  if (image.isNull()) {
    return image;
  }

  QImage result = image.convertToFormat(QImage::Format_ARGB32);

  int w = result.width();
  int h = result.height();
  QImage source = result;

  for (int y = 1; y < h - 1; ++y) {
    for (int x = 1; x < w - 1; ++x) {
      int r = 0, g = 0, b = 0;

      QRgb p0 = source.pixel(x, y);     // Center
      QRgb p1 = source.pixel(x, y - 1); // Top
      QRgb p2 = source.pixel(x - 1, y); // Left
      QRgb p3 = source.pixel(x + 1, y); // Right
      QRgb p4 = source.pixel(x, y + 1); // Bottom

      r = 5 * qRed(p0) - qRed(p1) - qRed(p2) - qRed(p3) - qRed(p4);
      g = 5 * qGreen(p0) - qGreen(p1) - qGreen(p2) - qGreen(p3) - qGreen(p4);
      b = 5 * qBlue(p0) - qBlue(p1) - qBlue(p2) - qBlue(p3) - qBlue(p4);

      r = std::clamp(r, 0, 255);
      g = std::clamp(g, 0, 255);
      b = std::clamp(b, 0, 255);

      result.setPixel(x, y, qRgba(r, g, b, qAlpha(p0)));
    }
  }

  return result;
}
