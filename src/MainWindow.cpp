#include "MainWindow.h"
#include "ImageCanvas.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_canvas(new ImageCanvas(this)),
      m_statusLabel(new QLabel(this)), m_currentFilePath(),
      m_isModified(false) {
  setCentralWidget(m_canvas);
  setMinimumSize(800, 600);
  resize(1200, 800);

  setupMenuBar();
  setupStatusBar();
  updateWindowTitle();

  connect(m_canvas, &ImageCanvas::imageLoaded, this,
          &MainWindow::onImageLoaded);
  connect(m_canvas, &ImageCanvas::imageModified, this,
          &MainWindow::onImageModified);
}

void MainWindow::setupMenuBar() {
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  QAction *newAction = fileMenu->addAction(tr("&New"));
  newAction->setShortcut(QKeySequence::New);
  connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);

  QAction *openAction = fileMenu->addAction(tr("&Open..."));
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);

  fileMenu->addSeparator();

  QAction *saveAction = fileMenu->addAction(tr("&Save"));
  saveAction->setShortcut(QKeySequence::Save);
  connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);

  QAction *saveAsAction = fileMenu->addAction(tr("Save &As..."));
  saveAsAction->setShortcut(QKeySequence::SaveAs);
  connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveFileAs);

  fileMenu->addSeparator();

  QAction *closeAction = fileMenu->addAction(tr("&Close"));
  closeAction->setShortcut(QKeySequence::Close);
  connect(closeAction, &QAction::triggered, this, &MainWindow::onCloseFile);

  QAction *quitAction = fileMenu->addAction(tr("&Quit"));
  quitAction->setShortcut(QKeySequence::Quit);
  connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

  QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

  QAction *undoAction = editMenu->addAction(tr("&Undo"));
  undoAction->setShortcut(QKeySequence::Undo);
  undoAction->setEnabled(false);

  QAction *redoAction = editMenu->addAction(tr("&Redo"));
  redoAction->setShortcut(QKeySequence::Redo);
  redoAction->setEnabled(false);

  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

  QAction *zoomInAction = viewMenu->addAction(tr("Zoom &In"));
  zoomInAction->setShortcut(QKeySequence::ZoomIn);
  zoomInAction->setEnabled(false);

  QAction *zoomOutAction = viewMenu->addAction(tr("Zoom &Out"));
  zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  zoomOutAction->setEnabled(false);

  viewMenu->addSeparator();

  QAction *fitAction = viewMenu->addAction(tr("&Fit to Window"));
  fitAction->setEnabled(false);

  QAction *actualSizeAction = viewMenu->addAction(tr("&Actual Size"));
  actualSizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
  actualSizeAction->setEnabled(false);
}

void MainWindow::setupStatusBar() {
  m_statusLabel->setText(tr("Ready"));
  statusBar()->addWidget(m_statusLabel);
}

void MainWindow::updateWindowTitle() {
  QString title = "Picture";

  if (!m_currentFilePath.isEmpty()) {
    QFileInfo info(m_currentFilePath);
    title = info.fileName() + " - Picture";
  }

  if (m_isModified) {
    title.prepend("* ");
  }

  setWindowTitle(title);
}

void MainWindow::updateStatusBar() {
  if (m_canvas->hasImage()) {
    QSize size = m_canvas->imageSize();
    m_statusLabel->setText(
        tr("%1 x %2 pixels").arg(size.width()).arg(size.height()));
  } else {
    m_statusLabel->setText(tr("Ready"));
  }
}

bool MainWindow::maybeSave() {
  if (!m_isModified) {
    return true;
  }

  QMessageBox::StandardButton result = QMessageBox::warning(
      this, tr("Unsaved Changes"),
      tr("The image has been modified.\nDo you want to save your changes?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

  if (result == QMessageBox::Save) {
    onSaveFile();
    return !m_isModified;
  } else if (result == QMessageBox::Cancel) {
    return false;
  }

  return true;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::onNewFile() {
  if (!maybeSave()) {
    return;
  }

  m_canvas->clearImage();
  m_currentFilePath.clear();
  m_isModified = false;
  updateWindowTitle();
  updateStatusBar();
}

void MainWindow::onOpenFile() {
  if (!maybeSave()) {
    return;
  }

  QString filter = tr(
      "Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff *.webp);;All Files (*)");
  QString filePath =
      QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), filter);

  if (filePath.isEmpty()) {
    return;
  }

  if (m_canvas->loadImage(filePath)) {
    m_currentFilePath = filePath;
    m_isModified = false;
    updateWindowTitle();
    updateStatusBar();
  } else {
    QMessageBox::critical(this, tr("Error"),
                          tr("Failed to open image:\n%1").arg(filePath));
  }
}

void MainWindow::onSaveFile() {
  if (m_currentFilePath.isEmpty()) {
    onSaveFileAs();
    return;
  }

  if (m_canvas->saveImage(m_currentFilePath)) {
    m_isModified = false;
    updateWindowTitle();
  } else {
    QMessageBox::critical(
        this, tr("Error"),
        tr("Failed to save image:\n%1").arg(m_currentFilePath));
  }
}

void MainWindow::onSaveFileAs() {
  if (!m_canvas->hasImage()) {
    return;
  }

  QString filter = tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image As"),
                                                  QString(), filter);

  if (filePath.isEmpty()) {
    return;
  }

  if (m_canvas->saveImage(filePath)) {
    m_currentFilePath = filePath;
    m_isModified = false;
    updateWindowTitle();
  } else {
    QMessageBox::critical(this, tr("Error"),
                          tr("Failed to save image:\n%1").arg(filePath));
  }
}

void MainWindow::onCloseFile() {
  if (!maybeSave()) {
    return;
  }

  m_canvas->clearImage();
  m_currentFilePath.clear();
  m_isModified = false;
  updateWindowTitle();
  updateStatusBar();
}

void MainWindow::onImageLoaded(const QString &path) {
  Q_UNUSED(path);
  updateStatusBar();
}

void MainWindow::onImageModified() {
  m_isModified = true;
  updateWindowTitle();
}
