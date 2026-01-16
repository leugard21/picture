#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QImage>
#include <QPixmap>
#include <QWidget>

class ImageCanvas : public QWidget {
  Q_OBJECT

public:
  explicit ImageCanvas(QWidget *parent = nullptr);
  ~ImageCanvas() override = default;

  bool loadImage(const QString &path);
  bool saveImage(const QString &path);
  void clearImage();

  [[nodiscard]] QImage getImage() const;
  [[nodiscard]] bool hasImage() const;
  [[nodiscard]] QSize imageSize() const;

signals:
  void imageLoaded(const QString &path);
  void imageSaved(const QString &path);
  void imageModified();

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void updateDisplayPixmap();
  void drawCheckerboard(QPainter &painter, const QRect &rect);

  QImage m_image;
  QPixmap m_displayPixmap;
  qreal m_zoomLevel;
  QPoint m_panOffset;
};

#endif
