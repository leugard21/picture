#ifndef DRAWINGTOOL_H
#define DRAWINGTOOL_H

#include <QColor>
#include <QImage>
#include <QPoint>

class DrawingTool {
public:
  DrawingTool();
  virtual ~DrawingTool() = default;

  virtual void onPress(QImage &image, const QPoint &pos) = 0;
  virtual void onMove(QImage &image, const QPoint &pos) = 0;
  virtual void onRelease(QImage &image, const QPoint &pos) = 0;

  void setColor(const QColor &color);
  QColor color() const;

  void setSize(int size);
  int size() const;

  void setOpacity(qreal opacity);
  qreal opacity() const;

protected:
  QColor m_color;
  int m_size;
  qreal m_opacity;
  QPoint m_lastPos;
};

#endif
