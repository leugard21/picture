#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageCanvas;
class QLabel;

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
  void onImageLoaded(const QString &path);
  void onImageModified();

private:
  void setupMenuBar();
  void setupStatusBar();
  void updateWindowTitle();
  void updateStatusBar();
  bool maybeSave();

  ImageCanvas *m_canvas;
  QLabel *m_statusLabel;
  QString m_currentFilePath;
  bool m_isModified;
};

#endif
