#include "BrushTool.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

BrushTool::BrushTool() : DrawingTool(), m_hardness(0.5) {}

void BrushTool::onPress(QImage &image, const QPoint &pos) {
  m_lastPos = pos;
  drawBrushDab(image, pos);
}

void BrushTool::onMove(QImage &image, const QPoint &pos) {
  drawLine(image, m_lastPos, pos);
  m_lastPos = pos;
}

void BrushTool::onRelease(QImage &image, const QPoint &pos) {
  Q_UNUSED(image);
  Q_UNUSED(pos);
}

void BrushTool::setHardness(qreal hardness) {
  m_hardness = std::clamp(hardness, 0.0, 1.0);
}

qreal BrushTool::hardness() const { return m_hardness; }

void BrushTool::drawLine(QImage &image, const QPoint &from, const QPoint &to) {
  int dx = to.x() - from.x();
  int dy = to.y() - from.y();
  int steps = std::max(std::abs(dx), std::abs(dy));

  if (steps == 0) {
    drawBrushDab(image, to);
    return;
  }

  for (int i = 0; i <= steps; ++i) {
    qreal t = static_cast<qreal>(i) / steps;
    int x = from.x() + t * dx;
    int y = from.y() + t * dy;
    drawBrushDab(image, QPoint(x, y));
  }
}

void BrushTool::drawBrushDab(QImage &image, const QPoint &pos) {
  if (image.isNull())
    return;

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  QRadialGradient gradient(pos, m_size / 2.0);

  QColor centerColor = m_color;
  centerColor.setAlphaF(m_opacity);

  QColor edgeColor = m_color;
  edgeColor.setAlphaF(m_opacity * (1.0 - m_hardness));

  gradient.setColorAt(0, centerColor);
  gradient.setColorAt(m_hardness, centerColor);
  gradient.setColorAt(1, edgeColor);

  painter.setBrush(gradient);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(pos, m_size / 2, m_size / 2);
}
