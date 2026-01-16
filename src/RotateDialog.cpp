#include "RotateDialog.h"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RotateDialog::RotateDialog(QWidget *parent)
    : QDialog(parent), m_angleSpin(nullptr), m_colorButton(nullptr),
      m_backgroundColor(Qt::transparent) {
  setWindowTitle(tr("Rotate Image"));
  setModal(true);
  setupUi();
}

qreal RotateDialog::angle() const { return m_angleSpin->value(); }

QColor RotateDialog::backgroundColor() const { return m_backgroundColor; }

void RotateDialog::onChooseColor() {
  QColor color =
      QColorDialog::getColor(m_backgroundColor, this, tr("Background Color"),
                             QColorDialog::ShowAlphaChannel);
  if (color.isValid()) {
    m_backgroundColor = color;
    QString style = QString("background-color: %1; border: 1px solid gray;")
                        .arg(color.name());
    m_colorButton->setStyleSheet(style);
  }
}

void RotateDialog::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);

  auto *formLayout = new QFormLayout();

  m_angleSpin = new QDoubleSpinBox();
  m_angleSpin->setRange(-180.0, 180.0);
  m_angleSpin->setDecimals(1);
  m_angleSpin->setSuffix(tr(" degrees"));
  m_angleSpin->setValue(0.0);
  formLayout->addRow(tr("Angle:"), m_angleSpin);

  auto *colorLayout = new QHBoxLayout();
  m_colorButton = new QPushButton();
  m_colorButton->setFixedSize(60, 24);
  m_colorButton->setStyleSheet(
      "background-color: transparent; border: 1px solid gray;");
  connect(m_colorButton, &QPushButton::clicked, this,
          &RotateDialog::onChooseColor);
  colorLayout->addWidget(m_colorButton);
  colorLayout->addWidget(new QLabel(tr("(for corners)")));
  colorLayout->addStretch();
  formLayout->addRow(tr("Background:"), colorLayout);

  mainLayout->addLayout(formLayout);

  auto *buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  mainLayout->addWidget(buttonBox);

  setFixedSize(sizeHint());
}
