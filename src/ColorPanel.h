#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QColor>
#include <QWidget>

class QPushButton;

class ColorPanel : public QWidget {
  Q_OBJECT

public:
  explicit ColorPanel(QWidget *parent = nullptr);
  ~ColorPanel() override = default;

  QColor foregroundColor() const;
  QColor backgroundColor() const;

signals:
  void foregroundColorChanged(const QColor &color);
  void backgroundColorChanged(const QColor &color);

public slots:
  void setForegroundColor(const QColor &color);
  void setBackgroundColor(const QColor &color);

private slots:
  void onForegroundClicked();
  void onBackgroundClicked();
  void onSwapColors();

private:
  void setupUi();
  void updateColorButtons();

  QColor m_foregroundColor;
  QColor m_backgroundColor;

  QPushButton *m_foregroundBtn;
  QPushButton *m_backgroundBtn;
  QPushButton *m_swapBtn;
};

#endif
