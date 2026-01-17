#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <memory>
#include <vector>

class CropOverlay;
class Layer;

class ImageCanvas : public QWidget {
  Q_OBJECT

public:
  static constexpr qreal MinZoom = 0.1;
  static constexpr qreal MaxZoom = 10.0;
  static constexpr qreal ZoomStep = 1.25;

  enum class FilterType { Grayscale, Sepia, Invert, Blur, Sharpen };

  explicit ImageCanvas(QWidget *parent = nullptr);
  ~ImageCanvas() override = default;

  bool loadProject(const QString &path);
  bool saveProject(const QString &path);
  void clearProject();

  void addLayer(const QImage &image, const QString &name);
  void removeLayer(int index);
  void moveLayerUp(int index);
  void moveLayerDown(int index);
  void duplicateLayer(int index);
  void setActiveLayer(int index);
  int activeLayerIndex() const;
  void setLayerVisibility(int index, bool visible);
  void setLayerOpacity(int index, qreal opacity);
  void setLayerBlendMode(int index, int mode);

  std::shared_ptr<Layer> activeLayer();
  const std::vector<std::shared_ptr<Layer>> &layers() const;

  [[nodiscard]] QImage getFlattenedImage() const;
  [[nodiscard]] bool hasImage() const;
  [[nodiscard]] QSize imageSize() const;

  void setZoomLevel(qreal level);
  [[nodiscard]] qreal zoomLevel() const;
  void zoomIn();
  void zoomOut();
  void fitToWindow();
  void actualSize();
  void resetPan();

  void resizeImage(const QSize &newSize, Qt::TransformationMode mode);

  void startCrop();
  void applyCrop();
  void cancelCrop();
  [[nodiscard]] bool isCropping() const;

  void rotate90CW();
  void rotate90CCW();
  void rotate180();
  void rotateByAngle(qreal degrees, const QColor &background);

  void flipHorizontal();
  void flipVertical();

  void startAdjustmentMode();
  void setPreviewAdjustments(int brightness, int contrast, int saturation,
                             int hue);
  void applyAdjustments();
  void cancelAdjustments();
  [[nodiscard]] bool isAdjusting() const;

  void applyFilter(FilterType type);

signals:
  void imageLoaded(const QString &path);
  void imageSaved(const QString &path);
  void imageModified();
  void zoomChanged(qreal level);
  void cropModeChanged(bool cropping);
  void adjustmentModeChanged(bool adjusting);

  void layerAdded(const QString &name, bool visible);
  void layerRemoved(int index);
  void layerMoved(int from, int to);
  void activeLayerChanged(int index);

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  void updateDisplayPixmap();
  void drawCheckerboard(QPainter &painter, const QRect &rect);
  void zoomAtPoint(qreal factor, const QPoint &point);
  void constrainPan();
  QRect currentImageRect() const;
  void updateCropOverlay();

  std::vector<std::shared_ptr<Layer>> m_layers;
  int m_activeLayerIndex;

  QImage m_originalLayerImage;
  QPixmap m_displayPixmap;
  qreal m_zoomLevel;
  QPoint m_panOffset;
  QPoint m_lastMousePos;
  bool m_isPanning;
  bool m_isAdjusting;
  CropOverlay *m_cropOverlay;
};

#endif
