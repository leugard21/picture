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

private slots:
  void onNewFile();
  void onOpenFile();
  void onSaveFile();
  void onSaveFileAs();
  void onCloseFile();
  void onResizeImage();
  void onZoomIn();
  void onZoomOut();
  void onFitToWindow();
  void onActualSize();
  void onImageLoaded(const QString &path);
  void onImageModified();
  void onZoomChanged(qreal level);

private:
  void setupMenuBar();
  void setupStatusBar();
  void updateWindowTitle();
  void updateStatusBar();
  void updateViewActions();
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
};

#endif
