#ifndef ADJUSTMENTSPANEL_H
#define ADJUSTMENTSPANEL_H

#include <QWidget>

class QSlider;
class QSpinBox;
class QPushButton;

class AdjustmentsPanel : public QWidget {
  Q_OBJECT

public:
  explicit AdjustmentsPanel(QWidget *parent = nullptr);
  ~AdjustmentsPanel() override = default;

  void reset();

signals:
  void adjustmentsChanged(int brightness, int contrast, int saturation,
                          int hue);
  void applyRequested();
  void resetRequested();

private slots:
  void onValueChanged();

private:
  void setupUi();
  QWidget *createSliderRow(const QString &label, QSlider *&slider,
                           QSpinBox *&spinBox, int min, int max);

  QSlider *m_brightnessSlider;
  QSlider *m_contrastSlider;
  QSlider *m_saturationSlider;
  QSlider *m_hueSlider;

  QSpinBox *m_brightnessSpin;
  QSpinBox *m_contrastSpin;
  QSpinBox *m_saturationSpin;
  QSpinBox *m_hueSpin;
};

#endif
