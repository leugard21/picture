#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageCanvas;
class QLabel;
class QAction;

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
  void onZoomIn();
  void onZoomOut();
  void onFitToWindow();
  void onActualSize();
  void onImageLoaded(const QString &path);
  void onImageModified();
  void onZoomChanged(qreal level);
  void onCropModeChanged(bool cropping);

private:
  void setupMenuBar();
  void setupStatusBar();
  void updateWindowTitle();
  void updateStatusBar();
  void updateViewActions();
  void updateImageActions();
  bool maybeSave();

  ImageCanvas *m_canvas;
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
};

#endif
