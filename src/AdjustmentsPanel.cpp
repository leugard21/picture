#include "AdjustmentsPanel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

AdjustmentsPanel::AdjustmentsPanel(QWidget *parent)
    : QWidget(parent), m_brightnessSlider(nullptr), m_contrastSlider(nullptr),
      m_saturationSlider(nullptr), m_hueSlider(nullptr),
      m_brightnessSpin(nullptr), m_contrastSpin(nullptr),
      m_saturationSpin(nullptr), m_hueSpin(nullptr) {
  setupUi();
}

void AdjustmentsPanel::reset() {
  m_brightnessSlider->setValue(0);
  m_contrastSlider->setValue(0);
  m_saturationSlider->setValue(0);
  m_hueSlider->setValue(0);
}

void AdjustmentsPanel::onValueChanged() {
  emit adjustmentsChanged(m_brightnessSlider->value(),
                          m_contrastSlider->value(),
                          m_saturationSlider->value(), m_hueSlider->value());
}

void AdjustmentsPanel::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(8, 8, 8, 8);
  mainLayout->setSpacing(12);

  auto *titleLabel = new QLabel(tr("Adjustments"));
  titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
  mainLayout->addWidget(titleLabel);

  mainLayout->addWidget(createSliderRow(tr("Brightness"), m_brightnessSlider,
                                        m_brightnessSpin, -100, 100));
  mainLayout->addWidget(createSliderRow(tr("Contrast"), m_contrastSlider,
                                        m_contrastSpin, -100, 100));
  mainLayout->addWidget(createSliderRow(tr("Saturation"), m_saturationSlider,
                                        m_saturationSpin, -100, 100));
  mainLayout->addWidget(
      createSliderRow(tr("Hue"), m_hueSlider, m_hueSpin, -180, 180));

  mainLayout->addStretch();

  auto *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(8);

  auto *resetButton = new QPushButton(tr("Reset"));
  connect(resetButton, &QPushButton::clicked, this, [this]() {
    reset();
    emit resetRequested();
  });
  buttonLayout->addWidget(resetButton);

  auto *applyButton = new QPushButton(tr("Apply"));
  applyButton->setDefault(true);
  connect(applyButton, &QPushButton::clicked, this,
          &AdjustmentsPanel::applyRequested);
  buttonLayout->addWidget(applyButton);

  mainLayout->addLayout(buttonLayout);

  connect(m_brightnessSlider, &QSlider::valueChanged, this,
          &AdjustmentsPanel::onValueChanged);
  connect(m_contrastSlider, &QSlider::valueChanged, this,
          &AdjustmentsPanel::onValueChanged);
  connect(m_saturationSlider, &QSlider::valueChanged, this,
          &AdjustmentsPanel::onValueChanged);
  connect(m_hueSlider, &QSlider::valueChanged, this,
          &AdjustmentsPanel::onValueChanged);
}

QWidget *AdjustmentsPanel::createSliderRow(const QString &label,
                                           QSlider *&slider, QSpinBox *&spinBox,
                                           int min, int max) {
  auto *container = new QWidget();
  auto *layout = new QVBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  auto *headerLayout = new QHBoxLayout();
  headerLayout->setContentsMargins(0, 0, 0, 0);

  auto *nameLabel = new QLabel(label);
  headerLayout->addWidget(nameLabel);

  headerLayout->addStretch();

  spinBox = new QSpinBox();
  spinBox->setRange(min, max);
  spinBox->setValue(0);
  spinBox->setFixedWidth(60);
  headerLayout->addWidget(spinBox);

  layout->addLayout(headerLayout);

  slider = new QSlider(Qt::Horizontal);
  slider->setRange(min, max);
  slider->setValue(0);
  slider->setTickPosition(QSlider::TicksBelow);
  slider->setTickInterval((max - min) / 4);
  layout->addWidget(slider);

  connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
  connect(spinBox, &QSpinBox::valueChanged, slider, &QSlider::setValue);

  return container;
}
