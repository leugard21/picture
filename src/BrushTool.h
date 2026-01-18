#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "DrawingTool.h"

class BrushTool : public DrawingTool {
public:
  BrushTool();
  ~BrushTool() override = default;

  void onPress(QImage &image, const QPoint &pos) override;
  void onMove(QImage &image, const QPoint &pos) override;
  void onRelease(QImage &image, const QPoint &pos) override;

  void setHardness(qreal hardness);
  qreal hardness() const;

private:
  void drawLine(QImage &image, const QPoint &from, const QPoint &to);
  void drawBrushDab(QImage &image, const QPoint &pos);

  qreal m_hardness;
};

#endif
