#include "ResizeDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

ResizeDialog::ResizeDialog(const QSize &originalSize, QWidget *parent)
    : QDialog(parent), m_originalSize(originalSize),
      m_aspectRatio(static_cast<qreal>(originalSize.width()) /
                    originalSize.height()),
      m_widthSpin(nullptr), m_heightSpin(nullptr), m_aspectCheck(nullptr),
      m_algorithmCombo(nullptr), m_originalLabel(nullptr),
      m_newSizeLabel(nullptr), m_updatingSize(false) {
  setWindowTitle(tr("Resize Image"));
  setModal(true);
  setupUi();
}

QSize ResizeDialog::newSize() const {
  return QSize(m_widthSpin->value(), m_heightSpin->value());
}

Qt::TransformationMode ResizeDialog::transformationMode() const {
  int index = m_algorithmCombo->currentIndex();
  switch (index) {
  case 0:
    return Qt::FastTransformation;
  case 1:
  case 2:
  default:
    return Qt::SmoothTransformation;
  }
}

void ResizeDialog::onWidthChanged(int value) {
  if (m_updatingSize) {
    return;
  }

  if (m_aspectCheck->isChecked()) {
    m_updatingSize = true;
    int newHeight = static_cast<int>(value / m_aspectRatio);
    m_heightSpin->setValue(newHeight);
    m_updatingSize = false;
  }

  m_newSizeLabel->setText(tr("%1 x %2 pixels")
                              .arg(m_widthSpin->value())
                              .arg(m_heightSpin->value()));
}

void ResizeDialog::onHeightChanged(int value) {
  if (m_updatingSize) {
    return;
  }

  if (m_aspectCheck->isChecked()) {
    m_updatingSize = true;
    int newWidth = static_cast<int>(value * m_aspectRatio);
    m_widthSpin->setValue(newWidth);
    m_updatingSize = false;
  }

  m_newSizeLabel->setText(tr("%1 x %2 pixels")
                              .arg(m_widthSpin->value())
                              .arg(m_heightSpin->value()));
}

void ResizeDialog::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);

  m_originalLabel = new QLabel(tr("Original: %1 x %2 pixels")
                                   .arg(m_originalSize.width())
                                   .arg(m_originalSize.height()));
  mainLayout->addWidget(m_originalLabel);

  auto *sizeGroup = new QGroupBox(tr("New Size"));
  auto *sizeLayout = new QFormLayout(sizeGroup);

  m_widthSpin = new QSpinBox();
  m_widthSpin->setRange(1, 32000);
  m_widthSpin->setValue(m_originalSize.width());
  m_widthSpin->setSuffix(tr(" px"));
  sizeLayout->addRow(tr("Width:"), m_widthSpin);

  m_heightSpin = new QSpinBox();
  m_heightSpin->setRange(1, 32000);
  m_heightSpin->setValue(m_originalSize.height());
  m_heightSpin->setSuffix(tr(" px"));
  sizeLayout->addRow(tr("Height:"), m_heightSpin);

  m_aspectCheck = new QCheckBox(tr("Maintain aspect ratio"));
  m_aspectCheck->setChecked(true);
  sizeLayout->addRow(m_aspectCheck);

  mainLayout->addWidget(sizeGroup);

  auto *optionsGroup = new QGroupBox(tr("Options"));
  auto *optionsLayout = new QFormLayout(optionsGroup);

  m_algorithmCombo = new QComboBox();
  m_algorithmCombo->addItem(tr("Nearest Neighbor (fast)"));
  m_algorithmCombo->addItem(tr("Bilinear"));
  m_algorithmCombo->addItem(tr("Bicubic (best quality)"));
  m_algorithmCombo->setCurrentIndex(2);
  optionsLayout->addRow(tr("Algorithm:"), m_algorithmCombo);

  mainLayout->addWidget(optionsGroup);

  m_newSizeLabel = new QLabel(tr("%1 x %2 pixels")
                                  .arg(m_originalSize.width())
                                  .arg(m_originalSize.height()));
  mainLayout->addWidget(m_newSizeLabel);

  auto *buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  mainLayout->addWidget(buttonBox);

  connect(m_widthSpin, &QSpinBox::valueChanged, this,
          &ResizeDialog::onWidthChanged);
  connect(m_heightSpin, &QSpinBox::valueChanged, this,
          &ResizeDialog::onHeightChanged);

  setFixedSize(sizeHint());
}
