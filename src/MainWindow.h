#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageCanvas;
class AdjustmentsPanel;
class LayersPanel;
class QLabel;
class QAction;
class QDockWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

protected:
  void closeEvent(QCloseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void onNewFile();
  void onOpenFile();
  void onSaveFile();
  void onSaveFileAs();
  void onCloseFile();
  void onResizeImage();
  void onCrop();
  void onApplyCrop();
  void onCancelCrop();
  void onRotate90CW();
  void onRotate90CCW();
  void onRotate180();
  void onRotateArbitrary();
  void onFlipHorizontal();
  void onFlipVertical();
  void onToggleAdjustments();
  void onAdjustmentsChanged(int brightness, int contrast, int saturation,
                            int hue);
  void onApplyAdjustments();
  void onResetAdjustments();
  void onFilterGrayscale();
  void onFilterSepia();
  void onFilterInvert();
  void onFilterBlur();
  void onFilterSharpen();
  void onZoomIn();
  void onZoomOut();
  void onFitToWindow();
  void onActualSize();
  void onImageLoaded(const QString &path);
  void onImageModified();
  void onZoomChanged(qreal level);
  void onCropModeChanged(bool cropping);
  void onAdjustmentModeChanged(bool adjusting);

  void onToggleLayers();
  void onLayerAdded(const QString &name, bool visible);
  void onLayerRemoved(int index);
  void onLayerMoved(int from, int to);
  void onActiveLayerChanged(int index);

private:
  void setupMenuBar();
  void setupStatusBar();
  void setupDockWidgets();
  void updateWindowTitle();
  void updateStatusBar();
  void updateViewActions();
  void updateImageActions();
  bool maybeSave();

  ImageCanvas *m_canvas;
  AdjustmentsPanel *m_adjustmentsPanel;
  LayersPanel *m_layersPanel;
  QDockWidget *m_adjustmentsDock;
  QDockWidget *m_layersDock;

  QLabel *m_statusLabel;
  QLabel *m_zoomLabel;
  QString m_currentFilePath;
  bool m_isModified;

  QAction *m_zoomInAction;
  QAction *m_zoomOutAction;
  QAction *m_fitToWindowAction;
  QAction *m_actualSizeAction;
  QAction *m_resizeAction;
  QAction *m_cropAction;
  QAction *m_rotate90CWAction;
  QAction *m_rotate90CCWAction;
  QAction *m_rotate180Action;
  QAction *m_rotateArbitraryAction;
  QAction *m_flipHorizontalAction;
  QAction *m_flipVerticalAction;
  QAction *m_adjustmentsAction;
  QAction *m_layersAction;
  QAction *m_filterGrayscaleAction;
  QAction *m_filterSepiaAction;
  QAction *m_filterInvertAction;
  QAction *m_filterBlurAction;
  QAction *m_filterSharpenAction;
};

#endif
