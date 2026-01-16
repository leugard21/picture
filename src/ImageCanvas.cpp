#include "ImageCanvas.h"

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>

ImageCanvas::ImageCanvas(QWidget *parent)
    : QWidget(parent), m_image(), m_displayPixmap(), m_zoomLevel(1.0),
      m_panOffset(0, 0) {
  setMinimumSize(200, 200);
  setAutoFillBackground(true);

  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor(45, 45, 45));
  setPalette(pal);
}

bool ImageCanvas::loadImage(const QString &path) {
  QImageReader reader(path);
  reader.setAutoTransform(true);

  QImage image = reader.read();
  if (image.isNull()) {
    return false;
  }

  m_image = image;
  m_zoomLevel = 1.0;
  m_panOffset = QPoint(0, 0);

  updateDisplayPixmap();
  update();

  emit imageLoaded(path);
  return true;
}

bool ImageCanvas::saveImage(const QString &path) {
  if (m_image.isNull()) {
    return false;
  }

  QImageWriter writer(path);

  QFileInfo info(path);
  QString suffix = info.suffix().toLower();

  if (suffix == "jpg" || suffix == "jpeg") {
    writer.setFormat("JPEG");
    writer.setQuality(95);
  } else if (suffix == "png") {
    writer.setFormat("PNG");
    writer.setCompression(9);
  } else if (suffix == "bmp") {
    writer.setFormat("BMP");
  }

  if (!writer.write(m_image)) {
    return false;
  }

  emit imageSaved(path);
  return true;
}

void ImageCanvas::clearImage() {
  m_image = QImage();
  m_displayPixmap = QPixmap();
  m_zoomLevel = 1.0;
  m_panOffset = QPoint(0, 0);
  update();
}

QImage ImageCanvas::getImage() const { return m_image; }

bool ImageCanvas::hasImage() const { return !m_image.isNull(); }

QSize ImageCanvas::imageSize() const { return m_image.size(); }

void ImageCanvas::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  if (m_image.isNull()) {
    painter.fillRect(rect(), palette().color(QPalette::Window));
    return;
  }

  QSize scaledSize = m_image.size() * m_zoomLevel;
  int x = (width() - scaledSize.width()) / 2 + m_panOffset.x();
  int y = (height() - scaledSize.height()) / 2 + m_panOffset.y();
  QRect imageRect(x, y, scaledSize.width(), scaledSize.height());

  painter.fillRect(rect(), palette().color(QPalette::Window));

  drawCheckerboard(painter, imageRect);

  if (!m_displayPixmap.isNull()) {
    painter.drawPixmap(imageRect, m_displayPixmap);
  }
}

void ImageCanvas::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  updateDisplayPixmap();
}

void ImageCanvas::updateDisplayPixmap() {
  if (m_image.isNull()) {
    m_displayPixmap = QPixmap();
    return;
  }

  QSize targetSize = m_image.size() * m_zoomLevel;
  m_displayPixmap = QPixmap::fromImage(m_image.scaled(
      targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageCanvas::drawCheckerboard(QPainter &painter, const QRect &rect) {
  const int cellSize = 10;
  const QColor light(200, 200, 200);
  const QColor dark(150, 150, 150);

  painter.save();
  painter.setClipRect(rect);

  int startX = rect.left() - (rect.left() % cellSize);
  int startY = rect.top() - (rect.top() % cellSize);

  for (int y = startY; y < rect.bottom(); y += cellSize) {
    for (int x = startX; x < rect.right(); x += cellSize) {
      bool isLight = ((x / cellSize) + (y / cellSize)) % 2 == 0;
      painter.fillRect(x, y, cellSize, cellSize, isLight ? light : dark);
    }
  }

  painter.restore();
}
