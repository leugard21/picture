#include "LayersPanel.h"
#include "Layer.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

LayersPanel::LayersPanel(QWidget *parent)
    : QWidget(parent), m_layerList(nullptr), m_opacitySlider(nullptr),
      m_opacitySpin(nullptr), m_blendModeCombo(nullptr), m_addLayerBtn(nullptr),
      m_removeLayerBtn(nullptr), m_duplicateLayerBtn(nullptr),
      m_moveUpBtn(nullptr), m_moveDownBtn(nullptr), m_updatingUI(false) {
  setupUi();
}

void LayersPanel::clear() { m_layerList->clear(); }

void LayersPanel::addLayer(const QString &name, bool visible) {
  m_updatingUI = true;
  auto *item = new QListWidgetItem(name);
  item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
  item->setCheckState(visible ? Qt::Checked : Qt::Unchecked);

  m_layerList->insertItem(0, item);
  m_layerList->setCurrentItem(item);
  m_updatingUI = false;
}

void LayersPanel::updateLayer(int index, const QString &name, bool visible) {
  int listIndex = m_layerList->count() - 1 - index;
  if (listIndex < 0 || listIndex >= m_layerList->count()) {
    return;
  }

  m_updatingUI = true;
  QListWidgetItem *item = m_layerList->item(listIndex);
  item->setText(name);
  item->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
  m_updatingUI = false;
}

void LayersPanel::selectLayer(int index) {
  int listIndex = m_layerList->count() - 1 - index;
  if (listIndex >= 0 && listIndex < m_layerList->count()) {
    m_layerList->setCurrentRow(listIndex);
  }
}

void LayersPanel::setOpacity(qreal opacity) {
  m_updatingUI = true;
  int val = static_cast<int>(opacity * 100);
  m_opacitySlider->setValue(val);
  m_opacitySpin->setValue(val);
  m_updatingUI = false;
}

void LayersPanel::setBlendMode(int mode) {
  m_updatingUI = true;

  int index = m_blendModeCombo->findData(mode);
  if (index != -1) {
    m_blendModeCombo->setCurrentIndex(index);
  }
  m_updatingUI = false;
}

void LayersPanel::onLayerItemChanged(QListWidgetItem *item) {
  if (m_updatingUI)
    return;

  int listIndex = m_layerList->row(item);
  int layerIndex = m_layerList->count() - 1 - listIndex;

  emit layerVisibilityChanged(layerIndex, item->checkState() == Qt::Checked);
}

void LayersPanel::onListSelectionChanged() {
  if (m_updatingUI)
    return;

  if (m_layerList->currentRow() != -1) {
    int layerIndex = m_layerList->count() - 1 - m_layerList->currentRow();
    emit activeLayerChanged(layerIndex);
    updateControls();
  }
}

void LayersPanel::setupUi() {
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setSpacing(8);

  auto *opacityLayout = new QHBoxLayout();
  opacityLayout->addWidget(new QLabel(tr("Opacity:")));
  m_opacitySlider = new QSlider(Qt::Horizontal);
  m_opacitySlider->setRange(0, 100);
  opacityLayout->addWidget(m_opacitySlider);

  m_opacitySpin = new QSpinBox();
  m_opacitySpin->setRange(0, 100);
  m_opacitySpin->setSuffix("%");
  opacityLayout->addWidget(m_opacitySpin);
  mainLayout->addLayout(opacityLayout);

  connect(m_opacitySlider, &QSlider::valueChanged, m_opacitySpin,
          &QSpinBox::setValue);
  connect(m_opacitySpin, &QSpinBox::valueChanged, m_opacitySlider,
          &QSlider::setValue);
  connect(m_opacitySlider, &QSlider::valueChanged, [this](int val) {
    if (!m_updatingUI && m_layerList->currentRow() != -1) {
      int layerIndex = m_layerList->count() - 1 - m_layerList->currentRow();
      emit layerOpacityChanged(layerIndex, val / 100.0);
    }
  });

  auto *blendLayout = new QHBoxLayout();
  blendLayout->addWidget(new QLabel(tr("Mode:")));
  m_blendModeCombo = new QComboBox();

  m_blendModeCombo->addItem("Normal", QPainter::CompositionMode_SourceOver);
  m_blendModeCombo->addItem("Multiply", QPainter::CompositionMode_Multiply);
  m_blendModeCombo->addItem("Screen", QPainter::CompositionMode_Screen);
  m_blendModeCombo->addItem("Overlay", QPainter::CompositionMode_Overlay);
  m_blendModeCombo->addItem("Darken", QPainter::CompositionMode_Darken);
  m_blendModeCombo->addItem("Lighten", QPainter::CompositionMode_Lighten);
  m_blendModeCombo->addItem("Color Dodge",
                            QPainter::CompositionMode_ColorDodge);
  m_blendModeCombo->addItem("Color Burn", QPainter::CompositionMode_ColorBurn);
  m_blendModeCombo->addItem("Hard Light", QPainter::CompositionMode_HardLight);
  m_blendModeCombo->addItem("Soft Light", QPainter::CompositionMode_SoftLight);
  m_blendModeCombo->addItem("Difference", QPainter::CompositionMode_Difference);
  m_blendModeCombo->addItem("Exclusion", QPainter::CompositionMode_Exclusion);

  blendLayout->addWidget(m_blendModeCombo);
  mainLayout->addLayout(blendLayout);

  connect(m_blendModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) {
            if (!m_updatingUI && m_layerList->currentRow() != -1) {
              int layerIndex =
                  m_layerList->count() - 1 - m_layerList->currentRow();
              int mode = m_blendModeCombo->itemData(index).toInt();
              emit layerBlendModeChanged(layerIndex, mode);
            }
          });

  m_layerList = new QListWidget();
  m_layerList->setDragDropMode(QAbstractItemView::InternalMove);
  connect(m_layerList, &QListWidget::itemChanged, this,
          &LayersPanel::onLayerItemChanged);
  connect(m_layerList, &QListWidget::itemSelectionChanged, this,
          &LayersPanel::onListSelectionChanged);
  mainLayout->addWidget(m_layerList);

  auto *toolbarLayout = new QHBoxLayout();
  toolbarLayout->setSpacing(2);

  m_addLayerBtn = new QPushButton("+");
  m_addLayerBtn->setToolTip(tr("Add New Layer"));

  m_removeLayerBtn = new QPushButton("-");
  m_removeLayerBtn->setToolTip(tr("Remove Layer"));
  connect(m_removeLayerBtn, &QPushButton::clicked, [this]() {
    if (m_layerList->currentRow() != -1) {
      int layerIndex = m_layerList->count() - 1 - m_layerList->currentRow();
      emit layerRemoved(layerIndex);
    }
  });

  m_moveUpBtn = new QPushButton("↑");
  m_moveUpBtn->setToolTip(tr("Move Up"));
  connect(m_moveUpBtn, &QPushButton::clicked, [this]() {
    if (m_layerList->currentRow() != -1) {
      int layerIndex = m_layerList->count() - 1 - m_layerList->currentRow();
      emit layerMovedUp(layerIndex);
    }
  });

  m_moveDownBtn = new QPushButton("↓");
  m_moveDownBtn->setToolTip(tr("Move Down"));
  connect(m_moveDownBtn, &QPushButton::clicked, [this]() {
    if (m_layerList->currentRow() != -1) {
      int layerIndex = m_layerList->count() - 1 - m_layerList->currentRow();
      emit layerMovedDown(layerIndex);
    }
  });

  toolbarLayout->addWidget(m_addLayerBtn);
  toolbarLayout->addWidget(m_removeLayerBtn);
  toolbarLayout->addWidget(m_moveUpBtn);
  toolbarLayout->addWidget(m_moveDownBtn);

  mainLayout->addLayout(toolbarLayout);

  updateControls();
}

void LayersPanel::updateControls() {
  bool hasSelection = m_layerList->currentRow() != -1;
  m_opacitySlider->setEnabled(hasSelection);
  m_opacitySpin->setEnabled(hasSelection);
  m_blendModeCombo->setEnabled(hasSelection);
  m_removeLayerBtn->setEnabled(hasSelection);
  m_moveUpBtn->setEnabled(hasSelection);
  m_moveDownBtn->setEnabled(hasSelection);
}
