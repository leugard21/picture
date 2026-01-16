#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QImage>
#include <QPixmap>
#include <QWidget>

class ImageCanvas : public QWidget {
  Q_OBJECT

public:
  static constexpr qreal MinZoom = 0.1;
  static constexpr qreal MaxZoom = 10.0;
  static constexpr qreal ZoomStep = 1.25;

  explicit ImageCanvas(QWidget *parent = nullptr);
  ~ImageCanvas() override = default;

  bool loadImage(const QString &path);
  bool saveImage(const QString &path);
  void clearImage();

  [[nodiscard]] QImage getImage() const;
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

signals:
  void imageLoaded(const QString &path);
  void imageSaved(const QString &path);
  void imageModified();
  void zoomChanged(qreal level);

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

  QImage m_image;
  QPixmap m_displayPixmap;
  qreal m_zoomLevel;
  QPoint m_panOffset;
  QPoint m_lastMousePos;
  bool m_isPanning;
};

#endif
