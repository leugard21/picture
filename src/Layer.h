#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QPainter>
#include <QString>

class Layer {
public:
    Layer(const QImage& image, const QString& name = "Layer");
    ~Layer() = default;

    const QImage& image() const;
    void setImage(const QImage& image);

    QString name() const;
    void setName(const QString& name);

    bool isVisible() const;
    void setVisible(bool visible);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    QPainter::CompositionMode blendMode() const;
    void setBlendMode(QPainter::CompositionMode mode);

    void render(QPainter& painter, const QRect& targetRect) const;

private:
    QImage m_image;
    QString m_name;
    bool m_visible;
    qreal m_opacity;
    QPainter::CompositionMode m_blendMode;
};

#endif
