#include "Layer.h"

Layer::Layer(const QImage &image, const QString &name)
    : m_image(image), m_name(name), m_visible(true), m_opacity(1.0),
      m_blendMode(QPainter::CompositionMode_SourceOver) {
  if (m_image.format() != QImage::Format_ARGB32_Premultiplied) {
    m_image = m_image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  }
}

const QImage &Layer::image() const { return m_image; }

void Layer::setImage(const QImage &image) {
  m_image = image;
  if (m_image.format() != QImage::Format_ARGB32_Premultiplied) {
    m_image = m_image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  }
}

QString Layer::name() const { return m_name; }

void Layer::setName(const QString &name) { m_name = name; }

bool Layer::isVisible() const { return m_visible; }

void Layer::setVisible(bool visible) { m_visible = visible; }

qreal Layer::opacity() const { return m_opacity; }

void Layer::setOpacity(qreal opacity) {
  m_opacity = std::clamp(opacity, 0.0, 1.0);
}

QPainter::CompositionMode Layer::blendMode() const { return m_blendMode; }

void Layer::setBlendMode(QPainter::CompositionMode mode) { m_blendMode = mode; }

void Layer::render(QPainter &painter, const QRect &targetRect) const {
  if (!m_visible || qFuzzyIsNull(m_opacity) || m_image.isNull()) {
    return;
  }

  painter.setOpacity(m_opacity);
  painter.setCompositionMode(m_blendMode);
  painter.drawImage(targetRect, m_image);
}
