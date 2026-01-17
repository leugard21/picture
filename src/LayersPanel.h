#ifndef LAYERSPANEL_H
#define LAYERSPANEL_H

#include <QWidget>

class QListWidget;
class QSlider;
class QSpinBox;
class QComboBox;
class QPushButton;
class QListWidgetItem;
class Layer;

class LayersPanel : public QWidget {
  Q_OBJECT

public:
  explicit LayersPanel(QWidget *parent = nullptr);
  ~LayersPanel() override = default;

  void clear();
  void addLayer(const QString &name, bool visible);
  void updateLayer(int index, const QString &name, bool visible);
  void selectLayer(int index);

signals:
  void activeLayerChanged(int index);
  void layerVisibilityChanged(int index, bool visible);
  void layerOpacityChanged(int index, qreal opacity);
  void layerBlendModeChanged(int index, int mode);
  void layerAddedSquared();
  void layerRemoved(int index);
  void layerMovedUp(int index);
  void layerMovedDown(int index);
  void layerDuplicated(int index);

public slots:
  void setOpacity(qreal opacity);
  void setBlendMode(int mode);

private slots:
  void onLayerItemChanged(QListWidgetItem *item);
  void onListSelectionChanged();

private:
  void setupUi();
  void updateControls();

  QListWidget *m_layerList;
  QSlider *m_opacitySlider;
  QSpinBox *m_opacitySpin;
  QComboBox *m_blendModeCombo;

  QPushButton *m_addLayerBtn;
  QPushButton *m_removeLayerBtn;
  QPushButton *m_duplicateLayerBtn;
  QPushButton *m_moveUpBtn;
  QPushButton *m_moveDownBtn;

  bool m_updatingUI;
};

#endif
