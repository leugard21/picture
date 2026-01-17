#include "ImageCanvas.h"
#include "CropOverlay.h"
#include "ImageProcessor.h"
#include "Layer.h"

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <algorithm>

const qreal ImageCanvas::MinZoom;
const qreal ImageCanvas::MaxZoom;
const qreal ImageCanvas::ZoomStep;

ImageCanvas::ImageCanvas(QWidget *parent)
    : QWidget(parent), m_layers(), m_activeLayerIndex(-1),
      m_originalLayerImage(), m_displayPixmap(), m_zoomLevel(1.0),
      m_panOffset(0, 0), m_lastMousePos(), m_isPanning(false),
      m_isAdjusting(false), m_cropOverlay(nullptr) {
  setMinimumSize(200, 200);
  setAutoFillBackground(true);
  setMouseTracking(true);

  QPalette pal = palette();
  pal.setColor(QPalette::Window, QColor(45, 45, 45));
  setPalette(pal);
}

bool ImageCanvas::loadProject(const QString &path) {
  QImageReader reader(path);
  reader.setAutoTransform(true);

  QImage image = reader.read();
  if (image.isNull()) {
    return false;
  }

  clearProject();
  addLayer(image, "Background");

  emit imageLoaded(path);
  emit zoomChanged(m_zoomLevel);
  return true;
}

bool ImageCanvas::saveProject(const QString &path) {
  if (!hasImage()) {
    return false;
  }

  QImage flattened = getFlattenedImage();
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

  if (!writer.write(flattened)) {
    return false;
  }

  emit imageSaved(path);
  return true;
}

void ImageCanvas::clearProject() {
  cancelCrop();
  m_layers.clear();
  m_activeLayerIndex = -1;
  m_displayPixmap = QPixmap();
  m_zoomLevel = 1.0;
  m_panOffset = QPoint(0, 0);
  update();
  emit zoomChanged(m_zoomLevel);
}

void ImageCanvas::addLayer(const QImage &image, const QString &name) {
  auto layer = std::make_shared<Layer>(image, name);
  m_layers.push_back(layer);

  int newIndex = static_cast<int>(m_layers.size()) - 1;
  setActiveLayer(newIndex);

  updateDisplayPixmap();
  update();

  emit layerAdded(name, true);
  emit activeLayerChanged(newIndex);
  emit imageModified();
}

void ImageCanvas::removeLayer(int index) {
  if (index < 0 || index >= static_cast<int>(m_layers.size()))
    return;

  m_layers.erase(m_layers.begin() + index);

  if (m_layers.empty()) {
    m_activeLayerIndex = -1;
  } else if (m_activeLayerIndex >= static_cast<int>(m_layers.size())) {
    m_activeLayerIndex = static_cast<int>(m_layers.size()) - 1;
  }

  updateDisplayPixmap();
  update();

  emit layerRemoved(index);
  emit activeLayerChanged(m_activeLayerIndex);
  emit imageModified();
}

void ImageCanvas::moveLayerUp(int index) {
  if (index < 0 || index >= static_cast<int>(m_layers.size()) - 1)
    return;

  std::swap(m_layers[index], m_layers[index + 1]);

  if (m_activeLayerIndex == index) {
    m_activeLayerIndex++;
  } else if (m_activeLayerIndex == index + 1) {
    m_activeLayerIndex--;
  }

  updateDisplayPixmap();
  update();

  emit layerMoved(index, index + 1);
  emit activeLayerChanged(m_activeLayerIndex);
  emit imageModified();
}

void ImageCanvas::moveLayerDown(int index) {
  if (index <= 0 || index >= static_cast<int>(m_layers.size()))
    return;

  std::swap(m_layers[index], m_layers[index - 1]);

  if (m_activeLayerIndex == index) {
    m_activeLayerIndex--;
  } else if (m_activeLayerIndex == index - 1) {
    m_activeLayerIndex++;
  }

  updateDisplayPixmap();
  update();

  emit layerMoved(index, index - 1);
  emit activeLayerChanged(m_activeLayerIndex);
  emit imageModified();
}

void ImageCanvas::duplicateLayer(int index) {
  if (index < 0 || index >= static_cast<int>(m_layers.size()))
    return;

  auto source = m_layers[index];
  auto copy =
      std::make_shared<Layer>(source->image(), source->name() + " Copy");
  copy->setOpacity(source->opacity());
  copy->setVisible(source->isVisible());
  copy->setBlendMode(source->blendMode());

  m_layers.insert(m_layers.begin() + index + 1, copy);

  updateDisplayPixmap();
  update();

  emit layerAdded(copy->name(), copy->isVisible());
  setActiveLayer(index + 1);

  emit activeLayerChanged(m_activeLayerIndex);
  emit imageModified();
}

void ImageCanvas::setActiveLayer(int index) {
  if (index >= 0 && index < static_cast<int>(m_layers.size())) {
    m_activeLayerIndex = index;
  } else {
    m_activeLayerIndex = -1;
  }
}

int ImageCanvas::activeLayerIndex() const { return m_activeLayerIndex; }

void ImageCanvas::setLayerVisibility(int index, bool visible) {
  if (index >= 0 && index < static_cast<int>(m_layers.size())) {
    m_layers[index]->setVisible(visible);
    updateDisplayPixmap();
    update();
    emit imageModified();
  }
}

void ImageCanvas::setLayerOpacity(int index, qreal opacity) {
  if (index >= 0 && index < static_cast<int>(m_layers.size())) {
    m_layers[index]->setOpacity(opacity);
    updateDisplayPixmap();
    update();
    emit imageModified();
  }
}

void ImageCanvas::setLayerBlendMode(int index, int mode) {
  if (index >= 0 && index < static_cast<int>(m_layers.size())) {
    m_layers[index]->setBlendMode(static_cast<QPainter::CompositionMode>(mode));
    updateDisplayPixmap();
    update();
    emit imageModified();
  }
}

std::shared_ptr<Layer> ImageCanvas::activeLayer() {
  if (m_activeLayerIndex >= 0 &&
      m_activeLayerIndex < static_cast<int>(m_layers.size())) {
    return m_layers[m_activeLayerIndex];
  }
  return nullptr;
}

const std::vector<std::shared_ptr<Layer>> &ImageCanvas::layers() const {
  return m_layers;
}

QImage ImageCanvas::getFlattenedImage() const {
  if (m_layers.empty())
    return QImage();

  QSize size = m_layers[0]->image().size();

  QImage result(size, QImage::Format_ARGB32_Premultiplied);
  result.fill(Qt::transparent);

  QPainter painter(&result);
  QRect rect(QPoint(0, 0), size);

  for (const auto &layer : m_layers) {
    layer->render(painter, rect);
  }

  return result;
}

bool ImageCanvas::hasImage() const { return !m_layers.empty(); }

QSize ImageCanvas::imageSize() const {
  if (m_layers.empty())
    return QSize(0, 0);
  return m_layers[0]->image().size();
}

void ImageCanvas::setZoomLevel(qreal level) {
  level = std::clamp(level, MinZoom, MaxZoom);
  if (qFuzzyCompare(level, m_zoomLevel)) {
    return;
  }

  m_zoomLevel = level;
  updateDisplayPixmap();
  constrainPan();
  updateCropOverlay();
  update();
  emit zoomChanged(m_zoomLevel);
}

qreal ImageCanvas::zoomLevel() const { return m_zoomLevel; }

void ImageCanvas::zoomIn() { setZoomLevel(m_zoomLevel * ZoomStep); }

void ImageCanvas::zoomOut() { setZoomLevel(m_zoomLevel / ZoomStep); }

void ImageCanvas::fitToWindow() {
  if (!hasImage()) {
    return;
  }

  QSize size = imageSize();
  qreal scaleX = static_cast<qreal>(width()) / size.width();
  qreal scaleY = static_cast<qreal>(height()) / size.height();
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
  if (!hasImage() || newSize.isEmpty()) {
    return;
  }

  for (auto &layer : m_layers) {
    layer->setImage(
        layer->image().scaled(newSize, Qt::IgnoreAspectRatio, mode));
  }

  m_zoomLevel = 1.0;
  m_panOffset = QPoint(0, 0);

  updateDisplayPixmap();
  update();

  emit imageModified();
  emit zoomChanged(m_zoomLevel);
}

void ImageCanvas::startCrop() {
  if (!hasImage() || m_cropOverlay) {
    return;
  }

  m_cropOverlay = new CropOverlay(this);
  m_cropOverlay->setImageRect(currentImageRect());
  m_cropOverlay->setGeometry(rect());
  m_cropOverlay->show();
  m_cropOverlay->raise();

  emit cropModeChanged(true);
}

void ImageCanvas::applyCrop() {
  if (!m_cropOverlay || !hasImage()) {
    return;
  }

  QRect selection = m_cropOverlay->selection();
  QRect imageRect = currentImageRect();
  QSize size = imageSize();

  int srcX = static_cast<int>((selection.x() - imageRect.x()) / m_zoomLevel);
  int srcY = static_cast<int>((selection.y() - imageRect.y()) / m_zoomLevel);
  int srcW = static_cast<int>(selection.width() / m_zoomLevel);
  int srcH = static_cast<int>(selection.height() / m_zoomLevel);

  srcX = std::clamp(srcX, 0, size.width() - 1);
  srcY = std::clamp(srcY, 0, size.height() - 1);
  srcW = std::clamp(srcW, 1, size.width() - srcX);
  srcH = std::clamp(srcH, 1, size.height() - srcY);

  for (auto &layer : m_layers) {
    layer->setImage(layer->image().copy(srcX, srcY, srcW, srcH));
  }

  m_panOffset = QPoint(0, 0);

  delete m_cropOverlay;
  m_cropOverlay = nullptr;

  updateDisplayPixmap();
  update();

  emit imageModified();
  emit cropModeChanged(false);
}

void ImageCanvas::cancelCrop() {
  if (m_cropOverlay) {
    delete m_cropOverlay;
    m_cropOverlay = nullptr;
    emit cropModeChanged(false);
  }
}

bool ImageCanvas::isCropping() const { return m_cropOverlay != nullptr; }

void ImageCanvas::rotate90CW() {
  auto layer = activeLayer();
  if (!layer)
    return;

  QTransform transform;
  transform.rotate(90);
  layer->setImage(
      layer->image().transformed(transform, Qt::SmoothTransformation));

  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::rotate90CCW() {
  auto layer = activeLayer();
  if (!layer)
    return;

  QTransform transform;
  transform.rotate(-90);
  layer->setImage(
      layer->image().transformed(transform, Qt::SmoothTransformation));

  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::rotate180() {
  auto layer = activeLayer();
  if (!layer)
    return;

  QTransform transform;
  transform.rotate(180);
  layer->setImage(
      layer->image().transformed(transform, Qt::SmoothTransformation));

  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::rotateByAngle(qreal degrees, const QColor &background) {
  auto layer = activeLayer();
  if (!layer || qFuzzyIsNull(degrees))
    return;

  QTransform transform;
  transform.rotate(degrees);
  QImage rotated =
      layer->image().transformed(transform, Qt::SmoothTransformation);

  if (background.alpha() > 0) {
    QImage result(rotated.size(), QImage::Format_ARGB32_Premultiplied);
    result.fill(background);
    QPainter painter(&result);
    painter.drawImage(0, 0, rotated);
    painter.end();
    layer->setImage(result);
  } else {
    layer->setImage(rotated);
  }

  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::flipHorizontal() {
  auto layer = activeLayer();
  if (!layer)
    return;

  layer->setImage(layer->image().flipped(Qt::Horizontal));
  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::flipVertical() {
  auto layer = activeLayer();
  if (!layer)
    return;

  layer->setImage(layer->image().flipped(Qt::Vertical));
  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::startAdjustmentMode() {
  auto layer = activeLayer();
  if (!layer || m_isAdjusting) {
    return;
  }

  m_originalLayerImage = layer->image();
  m_isAdjusting = true;
  emit adjustmentModeChanged(true);
}

void ImageCanvas::setPreviewAdjustments(int brightness, int contrast,
                                        int saturation, int hue) {
  auto layer = activeLayer();
  if (!m_isAdjusting || m_originalLayerImage.isNull() || !layer) {
    return;
  }

  layer->setImage(ImageProcessor::applyAdjustments(
      m_originalLayerImage, brightness, contrast, saturation, hue));
  updateDisplayPixmap();
  update();
}

void ImageCanvas::applyAdjustments() {
  if (!m_isAdjusting) {
    return;
  }

  m_originalLayerImage = QImage();
  m_isAdjusting = false;
  emit imageModified();
  emit adjustmentModeChanged(false);
}

void ImageCanvas::cancelAdjustments() {
  auto layer = activeLayer();
  if (!m_isAdjusting || !layer) {
    return;
  }

  layer->setImage(m_originalLayerImage);
  m_originalLayerImage = QImage();
  m_isAdjusting = false;

  updateDisplayPixmap();
  update();
  emit adjustmentModeChanged(false);
}

bool ImageCanvas::isAdjusting() const { return m_isAdjusting; }

void ImageCanvas::applyFilter(FilterType type) {
  auto layer = activeLayer();
  if (!layer)
    return;

  QImage img = layer->image();
  switch (type) {
  case FilterType::Grayscale:
    img = ImageProcessor::applyGrayscale(img);
    break;
  case FilterType::Sepia:
    img = ImageProcessor::applySepia(img);
    break;
  case FilterType::Invert:
    img = ImageProcessor::applyInvert(img);
    break;
  case FilterType::Blur:
    img = ImageProcessor::applyBlur(img);
    break;
  case FilterType::Sharpen:
    img = ImageProcessor::applySharpen(img);
    break;
  }

  layer->setImage(img);
  updateDisplayPixmap();
  update();
  emit imageModified();
}

void ImageCanvas::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  if (!hasImage()) {
    painter.fillRect(rect(), palette().color(QPalette::Window));
    return;
  }

  QRect imageRect = currentImageRect();

  painter.fillRect(rect(), palette().color(QPalette::Window));
  drawCheckerboard(painter, imageRect);

  if (!m_displayPixmap.isNull()) {
    painter.drawPixmap(imageRect, m_displayPixmap);
  }
}

void ImageCanvas::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  updateDisplayPixmap();
  updateCropOverlay();
}

void ImageCanvas::wheelEvent(QWheelEvent *event) {
  if (!hasImage() || m_cropOverlay) {
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
  if (!hasImage() || m_cropOverlay) {
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
  if (!hasImage()) {
    m_displayPixmap = QPixmap();
    return;
  }

  QImage flattened = getFlattenedImage();

  QSize targetSize = flattened.size() * m_zoomLevel;
  m_displayPixmap = QPixmap::fromImage(flattened.scaled(
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

  QSize size = imageSize();
  QSize scaledSize = size * m_zoomLevel;
  int imgX = (width() - scaledSize.width()) / 2 + m_panOffset.x();
  int imgY = (height() - scaledSize.height()) / 2 + m_panOffset.y();

  QPointF relativePos(
      (point.x() - imgX) / static_cast<qreal>(scaledSize.width()),
      (point.y() - imgY) / static_cast<qreal>(scaledSize.height()));

  m_zoomLevel = newZoom;
  updateDisplayPixmap();

  QSize newScaledSize = size * m_zoomLevel;
  int newImgX = point.x() - relativePos.x() * newScaledSize.width();
  int newImgY = point.y() - relativePos.y() * newScaledSize.height();

  m_panOffset.setX(newImgX - (width() - newScaledSize.width()) / 2);
  m_panOffset.setY(newImgY - (height() - newScaledSize.height()) / 2);

  constrainPan();
  update();
  emit zoomChanged(m_zoomLevel);
}

void ImageCanvas::constrainPan() {
  if (!hasImage()) {
    return;
  }

  QSize scaledSize = imageSize() * m_zoomLevel;
  int maxPanX = std::max(0, (scaledSize.width() - width()) / 2 + 100);
  int maxPanY = std::max(0, (scaledSize.height() - height()) / 2 + 100);

  m_panOffset.setX(std::clamp(m_panOffset.x(), -maxPanX, maxPanX));
  m_panOffset.setY(std::clamp(m_panOffset.y(), -maxPanY, maxPanY));
}

QRect ImageCanvas::currentImageRect() const {
  if (!hasImage()) {
    return QRect();
  }

  QSize scaledSize = imageSize() * m_zoomLevel;
  int x = (width() - scaledSize.width()) / 2 + m_panOffset.x();
  int y = (height() - scaledSize.height()) / 2 + m_panOffset.y();
  return QRect(x, y, scaledSize.width(), scaledSize.height());
}

void ImageCanvas::updateCropOverlay() {
  if (m_cropOverlay) {
    m_cropOverlay->setGeometry(rect());
    m_cropOverlay->setImageRect(currentImageRect());
  }
}
