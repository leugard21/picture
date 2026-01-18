#include "ColorPanel.h"
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ColorPanel::ColorPanel(QWidget *parent)
    : QWidget(parent), m_foregroundColor(Qt::black),
      m_backgroundColor(Qt::white), m_foregroundBtn(nullptr),
      m_backgroundBtn(nullptr), m_swapBtn(nullptr) {
  setupUi();
}

void ColorPanel::setForegroundColor(const QColor &color) {
  if (m_foregroundColor != color) {
    m_foregroundColor = color;
    updateColorButtons();
    emit foregroundColorChanged(color);
  }
}

void ColorPanel::setBackgroundColor(const QColor &color) {
  if (m_backgroundColor != color) {
    m_backgroundColor = color;
    updateColorButtons();
    emit backgroundColorChanged(color);
  }
}

QColor ColorPanel::foregroundColor() const { return m_foregroundColor; }

QColor ColorPanel::backgroundColor() const { return m_backgroundColor; }

void ColorPanel::onForegroundClicked() {
  QColor color = QColorDialog::getColor(m_foregroundColor, this,
                                        tr("Choose Foreground Color"));
  if (color.isValid()) {
    setForegroundColor(color);
  }
}

void ColorPanel::onBackgroundClicked() {
  QColor color = QColorDialog::getColor(m_backgroundColor, this,
                                        tr("Choose Background Color"));
  if (color.isValid()) {
    setBackgroundColor(color);
  }
}

void ColorPanel::onSwapColors() {
  QColor temp = m_foregroundColor;
  setForegroundColor(m_backgroundColor);
  setBackgroundColor(temp);
}

void ColorPanel::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setSpacing(8);

  auto *label = new QLabel(tr("Colors:"));
  mainLayout->addWidget(label);

  auto *colorLayout = new QHBoxLayout();

  m_foregroundBtn = new QPushButton();
  m_foregroundBtn->setFixedSize(40, 40);
  m_foregroundBtn->setToolTip(tr("Foreground Color"));
  connect(m_foregroundBtn, &QPushButton::clicked, this,
          &ColorPanel::onForegroundClicked);
  colorLayout->addWidget(m_foregroundBtn);

  m_backgroundBtn = new QPushButton();
  m_backgroundBtn->setFixedSize(40, 40);
  m_backgroundBtn->setToolTip(tr("Background Color"));
  connect(m_backgroundBtn, &QPushButton::clicked, this,
          &ColorPanel::onBackgroundClicked);
  colorLayout->addWidget(m_backgroundBtn);

  m_swapBtn = new QPushButton("⇄");
  m_swapBtn->setFixedSize(30, 30);
  m_swapBtn->setToolTip(tr("Swap Colors"));
  connect(m_swapBtn, &QPushButton::clicked, this, &ColorPanel::onSwapColors);
  colorLayout->addWidget(m_swapBtn);

  colorLayout->addStretch();
  mainLayout->addLayout(colorLayout);
  mainLayout->addStretch();

  updateColorButtons();
}

void ColorPanel::updateColorButtons() {
  if (m_foregroundBtn) {
    m_foregroundBtn->setStyleSheet(
        QString("QPushButton { background-color: %1; border: 2px solid #333; }")
            .arg(m_foregroundColor.name()));
  }

  if (m_backgroundBtn) {
    m_backgroundBtn->setStyleSheet(
        QString("QPushButton { background-color: %1; border: 2px solid #333; }")
            .arg(m_backgroundColor.name()));
  }
}
