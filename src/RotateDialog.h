#ifndef ROTATEDIALOG_H
#define ROTATEDIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QPushButton;
class QLabel;

class RotateDialog : public QDialog {
  Q_OBJECT

public:
  explicit RotateDialog(QWidget *parent = nullptr);
  ~RotateDialog() override = default;

  [[nodiscard]] qreal angle() const;
  [[nodiscard]] QColor backgroundColor() const;

private slots:
  void onChooseColor();

private:
  void setupUi();

  QDoubleSpinBox *m_angleSpin;
  QPushButton *m_colorButton;
  QColor m_backgroundColor;
};

#endif
