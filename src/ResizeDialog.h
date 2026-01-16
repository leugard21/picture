#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>

class QSpinBox;
class QCheckBox;
class QComboBox;
class QLabel;

class ResizeDialog : public QDialog {
  Q_OBJECT

public:
  explicit ResizeDialog(const QSize &originalSize, QWidget *parent = nullptr);
  ~ResizeDialog() override = default;

  [[nodiscard]] QSize newSize() const;
  [[nodiscard]] Qt::TransformationMode transformationMode() const;

private slots:
  void onWidthChanged(int value);
  void onHeightChanged(int value);

private:
  void setupUi();

  QSize m_originalSize;
  qreal m_aspectRatio;

  QSpinBox *m_widthSpin;
  QSpinBox *m_heightSpin;
  QCheckBox *m_aspectCheck;
  QComboBox *m_algorithmCombo;
  QLabel *m_originalLabel;
  QLabel *m_newSizeLabel;

  bool m_updatingSize;
};

#endif
