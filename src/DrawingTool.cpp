#include "DrawingTool.h"

DrawingTool::DrawingTool()
    : m_color(Qt::black), m_size(10), m_opacity(1.0), m_lastPos() {}

void DrawingTool::setColor(const QColor &color) { m_color = color; }

QColor DrawingTool::color() const { return m_color; }

void DrawingTool::setSize(int size) { m_size = std::max(1, size); }

int DrawingTool::size() const { return m_size; }

void DrawingTool::setOpacity(qreal opacity) {
  m_opacity = std::clamp(opacity, 0.0, 1.0);
}

qreal DrawingTool::opacity() const { return m_opacity; }
