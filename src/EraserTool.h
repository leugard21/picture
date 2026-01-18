#ifndef ERASERTOOL_H
#define ERASERTOOL_H

#include "DrawingTool.h"

class EraserTool : public DrawingTool {
public:
  EraserTool();
  ~EraserTool() override = default;

  void onPress(QImage &image, const QPoint &pos) override;
  void onMove(QImage &image, const QPoint &pos) override;
  void onRelease(QImage &image, const QPoint &pos) override;

private:
  void drawLine(QImage &image, const QPoint &from, const QPoint &to);
  void eraseDab(QImage &image, const QPoint &pos);
};

#endif
