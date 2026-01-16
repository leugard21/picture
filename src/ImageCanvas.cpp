#include "ImageCanvas.h"

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <algorithm>

ImageCanvas::ImageCanvas(QWidget *parent)
    : QWidget(parent), m_image(), m_displayPixmap(), m_zoomLevel(1.0),
      m_panOffset(0, 0), m_lastMousePos(), m_isPanning(false) {
  setMinimumSize(200, 200);
  setAutoFillBackground(true);
  setMouseTracking(true);

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
  emit zoomChanged(m_zoomLevel);
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
  emit zoomChanged(m_zoomLevel);
}

QImage ImageCanvas::getImage() const { return m_image; }

bool ImageCanvas::hasImage() const { return !m_image.isNull(); }

QSize ImageCanvas::imageSize() const { return m_image.size(); }

void ImageCanvas::setZoomLevel(qreal level) {
  level = std::clamp(level, MinZoom, MaxZoom);
  if (qFuzzyCompare(level, m_zoomLevel)) {
    return;
  }

  m_zoomLevel = level;
  updateDisplayPixmap();
  constrainPan();
  update();
  emit zoomChanged(m_zoomLevel);
}

qreal ImageCanvas::zoomLevel() const { return m_zoomLevel; }

void ImageCanvas::zoomIn() { setZoomLevel(m_zoomLevel * ZoomStep); }

void ImageCanvas::zoomOut() { setZoomLevel(m_zoomLevel / ZoomStep); }

void ImageCanvas::fitToWindow() {
  if (m_image.isNull()) {
    return;
  }

  qreal scaleX = static_cast<qreal>(width()) / m_image.width();
  qreal scaleY = static_cast<qreal>(height()) / m_image.height();
  qreal scale = std::min(scaleX, scaleY) * 0.95;

  m_panOffset = QPoint(0, 0);
  setZoomLevel(scale);
}

void ImageCanvas::actualSize() {
  m_panOffset = QPoint(0, 0);
  setZoomLevel(1.0);
}

void ImageCanvas::resetPan() {
  m_panOffset = QPoint(0, 0);
  update();
}

void ImageCanvas::resizeImage(const QSize &newSize,
                              Qt::TransformationMode mode) {
  if (m_image.isNull() || newSize.isEmpty()) {
    return;
  }

  m_image = m_image.scaled(newSize, Qt::IgnoreAspectRatio, mode);
  m_zoomLevel = 1.0;
  m_panOffset = QPoint(0, 0);

  updateDisplayPixmap();
  update();

  emit imageModified();
  emit zoomChanged(m_zoomLevel);
}

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

void ImageCanvas::wheelEvent(QWheelEvent *event) {
  if (m_image.isNull()) {
    event->ignore();
    return;
  }

  int delta = event->angleDelta().y();
  if (delta == 0) {
    event->ignore();
    return;
  }

  qreal factor = (delta > 0) ? ZoomStep : (1.0 / ZoomStep);
  zoomAtPoint(factor, event->position().toPoint());
  event->accept();
}

void ImageCanvas::mousePressEvent(QMouseEvent *event) {
  if (m_image.isNull()) {
    event->ignore();
    return;
  }

  if (event->button() == Qt::LeftButton ||
      event->button() == Qt::MiddleButton) {
    m_isPanning = true;
    m_lastMousePos = event->pos();
    setCursor(Qt::ClosedHandCursor);
    event->accept();
  } else {
    event->ignore();
  }
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event) {
  if (m_isPanning) {
    QPoint delta = event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();
    m_panOffset += delta;
    constrainPan();
    update();
    event->accept();
  } else {
    event->ignore();
  }
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event) {
  if (m_isPanning && (event->button() == Qt::LeftButton ||
                      event->button() == Qt::MiddleButton)) {
    m_isPanning = false;
    setCursor(Qt::ArrowCursor);
    event->accept();
  } else {
    event->ignore();
  }
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

void ImageCanvas::zoomAtPoint(qreal factor, const QPoint &point) {
  qreal newZoom = std::clamp(m_zoomLevel * factor, MinZoom, MaxZoom);
  if (qFuzzyCompare(newZoom, m_zoomLevel)) {
    return;
  }

  QSize scaledSize = m_image.size() * m_zoomLevel;
  int imgX = (width() - scaledSize.width()) / 2 + m_panOffset.x();
  int imgY = (height() - scaledSize.height()) / 2 + m_panOffset.y();

  QPointF relativePos(
      (point.x() - imgX) / static_cast<qreal>(scaledSize.width()),
      (point.y() - imgY) / static_cast<qreal>(scaledSize.height()));

  m_zoomLevel = newZoom;
  updateDisplayPixmap();

  QSize newScaledSize = m_image.size() * m_zoomLevel;
  int newImgX = point.x() - relativePos.x() * newScaledSize.width();
  int newImgY = point.y() - relativePos.y() * newScaledSize.height();

  m_panOffset.setX(newImgX - (width() - newScaledSize.width()) / 2);
  m_panOffset.setY(newImgY - (height() - newScaledSize.height()) / 2);

  constrainPan();
  update();
  emit zoomChanged(m_zoomLevel);
}

void ImageCanvas::constrainPan() {
  if (m_image.isNull()) {
    return;
  }

  QSize scaledSize = m_image.size() * m_zoomLevel;
  int maxPanX = std::max(0, (scaledSize.width() - width()) / 2 + 100);
  int maxPanY = std::max(0, (scaledSize.height() - height()) / 2 + 100);

  m_panOffset.setX(std::clamp(m_panOffset.x(), -maxPanX, maxPanX));
  m_panOffset.setY(std::clamp(m_panOffset.y(), -maxPanY, maxPanY));
}
