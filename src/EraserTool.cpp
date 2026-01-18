#include "EraserTool.h"
#include <QPainter>

EraserTool::EraserTool() : DrawingTool() {}

void EraserTool::onPress(QImage &image, const QPoint &pos) {
  m_lastPos = pos;
  eraseDab(image, pos);
}

void EraserTool::onMove(QImage &image, const QPoint &pos) {
  drawLine(image, m_lastPos, pos);
  m_lastPos = pos;
}

void EraserTool::onRelease(QImage &image, const QPoint &pos) {
  Q_UNUSED(image);
  Q_UNUSED(pos);
}

void EraserTool::drawLine(QImage &image, const QPoint &from, const QPoint &to) {
  int dx = to.x() - from.x();
  int dy = to.y() - from.y();
  int steps = std::max(std::abs(dx), std::abs(dy));

  if (steps == 0) {
    eraseDab(image, to);
    return;
  }

  for (int i = 0; i <= steps; ++i) {
    qreal t = static_cast<qreal>(i) / steps;
    int x = from.x() + t * dx;
    int y = from.y() + t * dy;
    eraseDab(image, QPoint(x, y));
  }
}

void EraserTool::eraseDab(QImage &image, const QPoint &pos) {
  if (image.isNull())
    return;

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);

  painter.setBrush(Qt::transparent);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(pos, m_size / 2, m_size / 2);
}
