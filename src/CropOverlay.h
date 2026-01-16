#ifndef CROPOVERLAY_H
#define CROPOVERLAY_H

#include <QWidget>

class CropOverlay : public QWidget {
  Q_OBJECT

public:
  enum class Handle {
    None,
    TopLeft,
    Top,
    TopRight,
    Right,
    BottomRight,
    Bottom,
    BottomLeft,
    Left,
    Move
  };

  explicit CropOverlay(QWidget *parent = nullptr);
  ~CropOverlay() override = default;

  void setImageRect(const QRect &rect);
  [[nodiscard]] QRect selection() const;
  void resetSelection();

signals:
  void selectionChanged(const QRect &selection);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  Handle handleAtPoint(const QPoint &point) const;
  QRect handleRect(Handle handle) const;
  void updateCursor(Handle handle);
  void constrainSelection();

  static constexpr int HandleSize = 8;
  static constexpr int MinSelectionSize = 10;

  QRect m_imageRect;
  QRect m_selection;
  Handle m_activeHandle;
  QPoint m_dragStartPos;
  QRect m_dragStartSelection;
};

#endif
