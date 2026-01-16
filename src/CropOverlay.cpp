#include "CropOverlay.h"

#include <QMouseEvent>
#include <QPainter>
#include <algorithm>

CropOverlay::CropOverlay(QWidget *parent)
    : QWidget(parent), m_imageRect(), m_selection(),
      m_activeHandle(Handle::None), m_dragStartPos(), m_dragStartSelection() {
  setMouseTracking(true);
  setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void CropOverlay::setImageRect(const QRect &rect) {
  m_imageRect = rect;
  resetSelection();
}

QRect CropOverlay::selection() const { return m_selection; }

void CropOverlay::resetSelection() {
  if (m_imageRect.isValid()) {
    int margin = std::min(m_imageRect.width(), m_imageRect.height()) / 8;
    m_selection = m_imageRect.adjusted(margin, margin, -margin, -margin);
  } else {
    m_selection = QRect();
  }
  update();
  emit selectionChanged(m_selection);
}

void CropOverlay::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);

  if (m_selection.isValid()) {
    QPen borderPen(Qt::white, 2, Qt::SolidLine);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_selection);

    QPen dashedPen(Qt::black, 2, Qt::DashLine);
    painter.setPen(dashedPen);
    painter.drawRect(m_selection);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);
    for (int i = static_cast<int>(Handle::TopLeft);
         i <= static_cast<int>(Handle::Left); ++i) {
      painter.drawRect(handleRect(static_cast<Handle>(i)));
    }
  }
}

void CropOverlay::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_activeHandle = handleAtPoint(event->pos());
    m_dragStartPos = event->pos();
    m_dragStartSelection = m_selection;
    event->accept();
  } else {
    event->ignore();
  }
}

void CropOverlay::mouseMoveEvent(QMouseEvent *event) {
  if (m_activeHandle != Handle::None) {
    QPoint delta = event->pos() - m_dragStartPos;
    QRect newSelection = m_dragStartSelection;

    switch (m_activeHandle) {
    case Handle::Move:
      newSelection.translate(delta);
      break;
    case Handle::TopLeft:
      newSelection.setTopLeft(m_dragStartSelection.topLeft() + delta);
      break;
    case Handle::Top:
      newSelection.setTop(m_dragStartSelection.top() + delta.y());
      break;
    case Handle::TopRight:
      newSelection.setTopRight(m_dragStartSelection.topRight() + delta);
      break;
    case Handle::Right:
      newSelection.setRight(m_dragStartSelection.right() + delta.x());
      break;
    case Handle::BottomRight:
      newSelection.setBottomRight(m_dragStartSelection.bottomRight() + delta);
      break;
    case Handle::Bottom:
      newSelection.setBottom(m_dragStartSelection.bottom() + delta.y());
      break;
    case Handle::BottomLeft:
      newSelection.setBottomLeft(m_dragStartSelection.bottomLeft() + delta);
      break;
    case Handle::Left:
      newSelection.setLeft(m_dragStartSelection.left() + delta.x());
      break;
    default:
      break;
    }

    m_selection = newSelection.normalized();
    constrainSelection();
    update();
    emit selectionChanged(m_selection);
    event->accept();
  } else {
    Handle handle = handleAtPoint(event->pos());
    updateCursor(handle);
    event->ignore();
  }
}

void CropOverlay::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    m_activeHandle = Handle::None;
    event->accept();
  } else {
    event->ignore();
  }
}

CropOverlay::Handle CropOverlay::handleAtPoint(const QPoint &point) const {
  for (int i = static_cast<int>(Handle::TopLeft);
       i <= static_cast<int>(Handle::Left); ++i) {
    Handle h = static_cast<Handle>(i);
    if (handleRect(h).contains(point)) {
      return h;
    }
  }

  if (m_selection.contains(point)) {
    return Handle::Move;
  }

  return Handle::None;
}

QRect CropOverlay::handleRect(Handle handle) const {
  int hs = HandleSize;
  int hhs = hs / 2;
  QPoint center;

  switch (handle) {
  case Handle::TopLeft:
    center = m_selection.topLeft();
    break;
  case Handle::Top:
    center = QPoint(m_selection.center().x(), m_selection.top());
    break;
  case Handle::TopRight:
    center = m_selection.topRight();
    break;
  case Handle::Right:
    center = QPoint(m_selection.right(), m_selection.center().y());
    break;
  case Handle::BottomRight:
    center = m_selection.bottomRight();
    break;
  case Handle::Bottom:
    center = QPoint(m_selection.center().x(), m_selection.bottom());
    break;
  case Handle::BottomLeft:
    center = m_selection.bottomLeft();
    break;
  case Handle::Left:
    center = QPoint(m_selection.left(), m_selection.center().y());
    break;
  default:
    return QRect();
  }

  return QRect(center.x() - hhs, center.y() - hhs, hs, hs);
}

void CropOverlay::updateCursor(Handle handle) {
  switch (handle) {
  case Handle::TopLeft:
  case Handle::BottomRight:
    setCursor(Qt::SizeFDiagCursor);
    break;
  case Handle::TopRight:
  case Handle::BottomLeft:
    setCursor(Qt::SizeBDiagCursor);
    break;
  case Handle::Top:
  case Handle::Bottom:
    setCursor(Qt::SizeVerCursor);
    break;
  case Handle::Left:
  case Handle::Right:
    setCursor(Qt::SizeHorCursor);
    break;
  case Handle::Move:
    setCursor(Qt::SizeAllCursor);
    break;
  default:
    setCursor(Qt::ArrowCursor);
    break;
  }
}

void CropOverlay::constrainSelection() {
  if (!m_imageRect.isValid()) {
    return;
  }

  if (m_selection.width() < MinSelectionSize) {
    m_selection.setWidth(MinSelectionSize);
  }
  if (m_selection.height() < MinSelectionSize) {
    m_selection.setHeight(MinSelectionSize);
  }

  if (m_selection.left() < m_imageRect.left()) {
    m_selection.moveLeft(m_imageRect.left());
  }
  if (m_selection.top() < m_imageRect.top()) {
    m_selection.moveTop(m_imageRect.top());
  }
  if (m_selection.right() > m_imageRect.right()) {
    m_selection.moveRight(m_imageRect.right());
  }
  if (m_selection.bottom() > m_imageRect.bottom()) {
    m_selection.moveBottom(m_imageRect.bottom());
  }
}
