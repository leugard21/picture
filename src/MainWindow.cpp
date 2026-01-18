#include "MainWindow.h"
#include "AdjustmentsPanel.h"
#include "ColorPanel.h"
#include "ImageCanvas.h"
#include "LayersPanel.h"
#include "ResizeDialog.h"
#include "RotateDialog.h"

#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_canvas(new ImageCanvas(this)),
      m_adjustmentsPanel(nullptr), m_layersPanel(nullptr),
      m_colorPanel(nullptr), m_adjustmentsDock(nullptr), m_layersDock(nullptr),
      m_colorDock(nullptr), m_statusLabel(new QLabel(this)),
      m_zoomLabel(new QLabel(this)), m_currentFilePath(), m_isModified(false),
      m_zoomInAction(nullptr), m_zoomOutAction(nullptr),
      m_fitToWindowAction(nullptr), m_actualSizeAction(nullptr),
      m_resizeAction(nullptr), m_cropAction(nullptr),
      m_rotate90CWAction(nullptr), m_rotate90CCWAction(nullptr),
      m_rotate180Action(nullptr), m_rotateArbitraryAction(nullptr),
      m_flipHorizontalAction(nullptr), m_flipVerticalAction(nullptr),
      m_adjustmentsAction(nullptr), m_layersAction(nullptr),
      m_filterGrayscaleAction(nullptr), m_filterSepiaAction(nullptr),
      m_filterInvertAction(nullptr), m_filterBlurAction(nullptr),
      m_filterSharpenAction(nullptr), m_toolBrushAction(nullptr),
      m_toolEraserAction(nullptr) {
  setCentralWidget(m_canvas);
  setMinimumSize(800, 600);
  resize(1200, 800);

  setupMenuBar();
  setupStatusBar();
  setupDockWidgets();
  updateWindowTitle();

  connect(m_canvas, &ImageCanvas::imageLoaded, this,
          &MainWindow::onImageLoaded);
  connect(m_canvas, &ImageCanvas::imageModified, this,
          &MainWindow::onImageModified);
  connect(m_canvas, &ImageCanvas::zoomChanged, this,
          &MainWindow::onZoomChanged);
  connect(m_canvas, &ImageCanvas::cropModeChanged, this,
          &MainWindow::onCropModeChanged);
  connect(m_canvas, &ImageCanvas::adjustmentModeChanged, this,
          &MainWindow::onAdjustmentModeChanged);

  connect(m_canvas, &ImageCanvas::activeLayerChanged, this,
          &MainWindow::onActiveLayerChanged);
  connect(m_canvas, &ImageCanvas::layerAdded, m_layersPanel,
          &LayersPanel::addLayer);
  connect(m_canvas, &ImageCanvas::layerRemoved, m_layersPanel,
          [this](int index) { m_layersPanel->updateLayer(index, "", false); });
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

  QMenu *imageMenu = menuBar()->addMenu(tr("&Image"));

  m_resizeAction = imageMenu->addAction(tr("&Resize..."));
  m_resizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
  m_resizeAction->setEnabled(false);
  connect(m_resizeAction, &QAction::triggered, this,
          &MainWindow::onResizeImage);

  m_cropAction = imageMenu->addAction(tr("&Crop"));
  m_cropAction->setShortcut(QKeySequence(Qt::Key_C));
  m_cropAction->setEnabled(false);
  connect(m_cropAction, &QAction::triggered, this, &MainWindow::onCrop);

  imageMenu->addSeparator();

  m_adjustmentsAction = imageMenu->addAction(tr("&Adjustments..."));
  m_adjustmentsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
  m_adjustmentsAction->setEnabled(false);
  m_adjustmentsAction->setCheckable(true);
  connect(m_adjustmentsAction, &QAction::triggered, this,
          &MainWindow::onToggleAdjustments);

  m_layersAction = imageMenu->addAction(tr("&Layers..."));
  m_layersAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
  m_layersAction->setEnabled(false);
  m_layersAction->setCheckable(true);
  connect(m_layersAction, &QAction::triggered, this,
          &MainWindow::onToggleLayers);

  imageMenu->addSeparator();

  QMenu *filtersMenu = imageMenu->addMenu(tr("&Filters"));

  m_filterGrayscaleAction = filtersMenu->addAction(tr("&Grayscale"));
  m_filterGrayscaleAction->setEnabled(false);
  connect(m_filterGrayscaleAction, &QAction::triggered, this,
          &MainWindow::onFilterGrayscale);

  m_filterSepiaAction = filtersMenu->addAction(tr("&Sepia"));
  m_filterSepiaAction->setEnabled(false);
  connect(m_filterSepiaAction, &QAction::triggered, this,
          &MainWindow::onFilterSepia);

  m_filterInvertAction = filtersMenu->addAction(tr("&Invert"));
  m_filterInvertAction->setEnabled(false);
  connect(m_filterInvertAction, &QAction::triggered, this,
          &MainWindow::onFilterInvert);

  filtersMenu->addSeparator();

  m_filterBlurAction = filtersMenu->addAction(tr("&Blur"));
  m_filterBlurAction->setEnabled(false);
  connect(m_filterBlurAction, &QAction::triggered, this,
          &MainWindow::onFilterBlur);

  m_filterSharpenAction = filtersMenu->addAction(tr("S&harpen"));
  m_filterSharpenAction->setEnabled(false);
  connect(m_filterSharpenAction, &QAction::triggered, this,
          &MainWindow::onFilterSharpen);

  imageMenu->addSeparator();

  QMenu *rotateMenu = imageMenu->addMenu(tr("&Rotate"));

  m_rotate90CWAction = rotateMenu->addAction(tr("90 &Clockwise"));
  m_rotate90CWAction->setShortcut(
      QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
  m_rotate90CWAction->setEnabled(false);
  connect(m_rotate90CWAction, &QAction::triggered, this,
          &MainWindow::onRotate90CW);

  m_rotate90CCWAction = rotateMenu->addAction(tr("90 Counter-Clock&wise"));
  m_rotate90CCWAction->setShortcut(
      QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
  m_rotate90CCWAction->setEnabled(false);
  connect(m_rotate90CCWAction, &QAction::triggered, this,
          &MainWindow::onRotate90CCW);

  m_rotate180Action = rotateMenu->addAction(tr("&180"));
  m_rotate180Action->setEnabled(false);
  connect(m_rotate180Action, &QAction::triggered, this,
          &MainWindow::onRotate180);

  rotateMenu->addSeparator();

  m_rotateArbitraryAction = rotateMenu->addAction(tr("&Arbitrary Angle..."));
  m_rotateArbitraryAction->setEnabled(false);
  connect(m_rotateArbitraryAction, &QAction::triggered, this,
          &MainWindow::onRotateArbitrary);

  QMenu *flipMenu = imageMenu->addMenu(tr("&Flip"));

  m_flipHorizontalAction = flipMenu->addAction(tr("&Horizontal"));
  m_flipHorizontalAction->setShortcut(QKeySequence(Qt::Key_H));
  m_flipHorizontalAction->setEnabled(false);
  connect(m_flipHorizontalAction, &QAction::triggered, this,
          &MainWindow::onFlipHorizontal);

  m_flipVerticalAction = flipMenu->addAction(tr("&Vertical"));
  m_flipVerticalAction->setShortcut(QKeySequence(Qt::Key_V));
  m_flipVerticalAction->setEnabled(false);
  connect(m_flipVerticalAction, &QAction::triggered, this,
          &MainWindow::onFlipVertical);

  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

  m_zoomInAction = viewMenu->addAction(tr("Zoom &In"));
  m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
  m_zoomInAction->setEnabled(false);
  connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::onZoomIn);

  m_zoomOutAction = viewMenu->addAction(tr("Zoom &Out"));
  m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  m_zoomOutAction->setEnabled(false);
  connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::onZoomOut);

  viewMenu->addSeparator();

  m_fitToWindowAction = viewMenu->addAction(tr("&Fit to Window"));
  m_fitToWindowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
  m_fitToWindowAction->setEnabled(false);
  connect(m_fitToWindowAction, &QAction::triggered, this,
          &MainWindow::onFitToWindow);

  m_actualSizeAction = viewMenu->addAction(tr("&Actual Size"));
  m_actualSizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
  m_actualSizeAction->setEnabled(false);
  connect(m_actualSizeAction, &QAction::triggered, this,
          &MainWindow::onActualSize);

  QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));

  m_toolBrushAction = toolsMenu->addAction(tr("&Brush"));
  m_toolBrushAction->setShortcut(QKeySequence(Qt::Key_B));
  m_toolBrushAction->setCheckable(true);
  connect(m_toolBrushAction, &QAction::triggered, this,
          &MainWindow::onToolBrush);

  m_toolEraserAction = toolsMenu->addAction(tr("&Eraser"));
  m_toolEraserAction->setShortcut(QKeySequence(Qt::Key_E));
  m_toolEraserAction->setCheckable(true);
  connect(m_toolEraserAction, &QAction::triggered, this,
          &MainWindow::onToolEraser);
}

void MainWindow::setupStatusBar() {
  m_statusLabel->setText(tr("Ready"));
  statusBar()->addWidget(m_statusLabel, 1);

  m_zoomLabel->setText(tr("100%"));
  m_zoomLabel->setMinimumWidth(60);
  m_zoomLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::setupDockWidgets() {
  m_adjustmentsPanel = new AdjustmentsPanel(this);
  m_adjustmentsDock = new QDockWidget(tr("Adjustments"), this);
  m_adjustmentsDock->setWidget(m_adjustmentsPanel);
  m_adjustmentsDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                     Qt::RightDockWidgetArea);
  m_adjustmentsDock->setVisible(false);
  addDockWidget(Qt::RightDockWidgetArea, m_adjustmentsDock);

  connect(m_adjustmentsPanel, &AdjustmentsPanel::adjustmentsChanged, this,
          &MainWindow::onAdjustmentsChanged);
  connect(m_adjustmentsPanel, &AdjustmentsPanel::applyRequested, this,
          &MainWindow::onApplyAdjustments);
  connect(m_adjustmentsPanel, &AdjustmentsPanel::resetRequested, this,
          &MainWindow::onResetAdjustments);

  connect(m_adjustmentsDock, &QDockWidget::visibilityChanged,
          [this](bool visible) {
            m_adjustmentsAction->setChecked(visible);
            if (visible && m_canvas->hasImage() && !m_canvas->isAdjusting()) {
              m_canvas->startAdjustmentMode();
            } else if (!visible && m_canvas->isAdjusting()) {
              m_canvas->cancelAdjustments();
              m_adjustmentsPanel->reset();
            }
          });

  m_layersPanel = new LayersPanel(this);
  m_layersDock = new QDockWidget(tr("Layers"), this);
  m_layersDock->setWidget(m_layersPanel);
  m_layersDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
  m_layersDock->setVisible(false);
  addDockWidget(Qt::RightDockWidgetArea, m_layersDock);

  connect(m_layersPanel, &LayersPanel::activeLayerChanged, m_canvas,
          &ImageCanvas::setActiveLayer);
  connect(m_layersPanel, &LayersPanel::layerVisibilityChanged, m_canvas,
          &ImageCanvas::setLayerVisibility);
  connect(m_layersPanel, &LayersPanel::layerOpacityChanged, m_canvas,
          &ImageCanvas::setLayerOpacity);
  connect(m_layersPanel, &LayersPanel::layerBlendModeChanged, m_canvas,
          &ImageCanvas::setLayerBlendMode);

  connect(m_layersPanel, &LayersPanel::layerRemoved, m_canvas,
          &ImageCanvas::removeLayer);
  connect(m_layersPanel, &LayersPanel::layerMovedUp, m_canvas,
          &ImageCanvas::moveLayerUp);
  connect(m_layersPanel, &LayersPanel::layerMovedDown, m_canvas,
          &ImageCanvas::moveLayerDown);

  connect(m_canvas, &ImageCanvas::layerAdded, m_layersPanel,
          &LayersPanel::addLayer);
  connect(m_canvas, &ImageCanvas::layerRemoved, [this](int index) {
    Q_UNUSED(index);
    m_layersPanel->clear();
  });

  connect(m_layersDock, &QDockWidget::visibilityChanged,
          [this](bool visible) { m_layersAction->setChecked(visible); });

  m_colorPanel = new ColorPanel(this);
  m_colorDock = new QDockWidget(tr("Colors"), this);
  m_colorDock->setWidget(m_colorPanel);
  m_colorDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                               Qt::RightDockWidgetArea);
  m_colorDock->setVisible(true);
  addDockWidget(Qt::RightDockWidgetArea, m_colorDock);

  connect(m_colorPanel, &ColorPanel::foregroundColorChanged, this,
          &MainWindow::onForegroundColorChanged);
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

void MainWindow::updateViewActions() {
  bool hasImage = m_canvas->hasImage();
  bool notCropping = !m_canvas->isCropping();

  m_zoomInAction->setEnabled(hasImage && notCropping);
  m_zoomOutAction->setEnabled(hasImage && notCropping);
  m_fitToWindowAction->setEnabled(hasImage && notCropping);
  m_actualSizeAction->setEnabled(hasImage && notCropping);
}

void MainWindow::updateImageActions() {
  bool hasImage = m_canvas->hasImage();
  bool notCropping = !m_canvas->isCropping();
  bool notAdjusting = !m_canvas->isAdjusting();

  m_resizeAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_cropAction->setEnabled(hasImage && notAdjusting);
  m_adjustmentsAction->setEnabled(hasImage && notCropping);
  m_layersAction->setEnabled(hasImage);

  m_rotate90CWAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_rotate90CCWAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_rotate180Action->setEnabled(hasImage && notCropping && notAdjusting);
  m_rotateArbitraryAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_flipHorizontalAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_flipVerticalAction->setEnabled(hasImage && notCropping && notAdjusting);

  m_filterGrayscaleAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_filterSepiaAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_filterInvertAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_filterBlurAction->setEnabled(hasImage && notCropping && notAdjusting);
  m_filterSharpenAction->setEnabled(hasImage && notCropping && notAdjusting);

  if (m_canvas->isCropping()) {
    m_cropAction->setText(tr("&Apply Crop"));
  } else {
    m_cropAction->setText(tr("&Crop"));
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

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (m_canvas->isCropping()) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
      onApplyCrop();
      event->accept();
      return;
    } else if (event->key() == Qt::Key_Escape) {
      onCancelCrop();
      event->accept();
      return;
    }
  }
  QMainWindow::keyPressEvent(event);
}

void MainWindow::onNewFile() {
  if (!maybeSave()) {
    return;
  }

  m_canvas->clearProject();
  m_currentFilePath.clear();
  m_isModified = false;
  m_adjustmentsDock->setVisible(false);
  m_layersPanel->clear();
  updateWindowTitle();
  updateStatusBar();
  updateViewActions();
  updateImageActions();
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

  if (m_canvas->loadProject(filePath)) {
    m_currentFilePath = filePath;
    m_isModified = false;
    m_adjustmentsDock->setVisible(false);
    m_layersPanel->clear();
    m_layersPanel->addLayer("Background", true);
    updateWindowTitle();
    updateStatusBar();
    updateViewActions();
    updateImageActions();
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

  if (m_canvas->saveProject(m_currentFilePath)) {
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

  if (m_canvas->saveProject(filePath)) {
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

  m_canvas->clearProject();
  m_currentFilePath.clear();
  m_isModified = false;
  m_adjustmentsDock->setVisible(false);
  m_layersPanel->clear();
  updateWindowTitle();
  updateStatusBar();
  updateViewActions();
  updateImageActions();
}

void MainWindow::onResizeImage() {
  if (!m_canvas->hasImage()) {
    return;
  }

  ResizeDialog dialog(m_canvas->imageSize(), this);
  if (dialog.exec() == QDialog::Accepted) {
    m_canvas->resizeImage(dialog.newSize(), dialog.transformationMode());
    updateStatusBar();
  }
}

void MainWindow::onCrop() {
  if (m_canvas->isCropping()) {
    onApplyCrop();
  } else {
    m_canvas->startCrop();
  }
}

void MainWindow::onApplyCrop() {
  m_canvas->applyCrop();
  updateStatusBar();
}

void MainWindow::onCancelCrop() { m_canvas->cancelCrop(); }

void MainWindow::onRotate90CW() {
  m_canvas->rotate90CW();
  updateStatusBar();
}

void MainWindow::onRotate90CCW() {
  m_canvas->rotate90CCW();
  updateStatusBar();
}

void MainWindow::onRotate180() { m_canvas->rotate180(); }

void MainWindow::onRotateArbitrary() {
  if (!m_canvas->hasImage()) {
    return;
  }

  RotateDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    m_canvas->rotateByAngle(dialog.angle(), dialog.backgroundColor());
    updateStatusBar();
  }
}

void MainWindow::onFlipHorizontal() { m_canvas->flipHorizontal(); }

void MainWindow::onFlipVertical() { m_canvas->flipVertical(); }

void MainWindow::onToggleAdjustments() {
  bool visible = !m_adjustmentsDock->isVisible();
  m_adjustmentsDock->setVisible(visible);
}

void MainWindow::onToggleLayers() {
  bool visible = !m_layersDock->isVisible();
  m_layersDock->setVisible(visible);
}

void MainWindow::onAdjustmentsChanged(int brightness, int contrast,
                                      int saturation, int hue) {
  m_canvas->setPreviewAdjustments(brightness, contrast, saturation, hue);
}

void MainWindow::onApplyAdjustments() {
  m_canvas->applyAdjustments();
  m_adjustmentsPanel->reset();
  m_adjustmentsDock->setVisible(false);
}

void MainWindow::onResetAdjustments() {
  m_canvas->setPreviewAdjustments(0, 0, 0, 0);
}

void MainWindow::onFilterGrayscale() {
  m_canvas->applyFilter(ImageCanvas::FilterType::Grayscale);
}

void MainWindow::onFilterSepia() {
  m_canvas->applyFilter(ImageCanvas::FilterType::Sepia);
}

void MainWindow::onFilterInvert() {
  m_canvas->applyFilter(ImageCanvas::FilterType::Invert);
}

void MainWindow::onFilterBlur() {
  m_canvas->applyFilter(ImageCanvas::FilterType::Blur);
}

void MainWindow::onFilterSharpen() {
  m_canvas->applyFilter(ImageCanvas::FilterType::Sharpen);
}

void MainWindow::onZoomIn() { m_canvas->zoomIn(); }

void MainWindow::onZoomOut() { m_canvas->zoomOut(); }

void MainWindow::onFitToWindow() { m_canvas->fitToWindow(); }

void MainWindow::onActualSize() { m_canvas->actualSize(); }

void MainWindow::onImageLoaded(const QString &path) {
  Q_UNUSED(path);
  updateStatusBar();
  updateViewActions();
  updateImageActions();
}

void MainWindow::onImageModified() {
  m_isModified = true;
  updateWindowTitle();
  updateStatusBar();
}

void MainWindow::onZoomChanged(qreal level) {
  int percentage = static_cast<int>(level * 100);
  m_zoomLabel->setText(tr("%1%").arg(percentage));
}

void MainWindow::onCropModeChanged(bool cropping) {
  Q_UNUSED(cropping);
  updateViewActions();
  updateImageActions();
}

void MainWindow::onAdjustmentModeChanged(bool adjusting) {
  Q_UNUSED(adjusting);
  updateViewActions();
  updateImageActions();
}

void MainWindow::onLayerAdded(const QString &name, bool visible) {
  Q_UNUSED(name);
  Q_UNUSED(visible);
}
void MainWindow::onLayerRemoved(int index) { Q_UNUSED(index); }
void MainWindow::onLayerMoved(int from, int to) {
  Q_UNUSED(from);
  Q_UNUSED(to);
}
void MainWindow::onActiveLayerChanged(int index) {
  m_layersPanel->selectLayer(index);
}

// Tool slots
void MainWindow::onToolBrush()
{
    m_canvas->setToolMode(ImageCanvas::ToolMode::Brush);
    m_toolBrushAction->setChecked(true);
    m_toolEraserAction->setChecked(false);
}

void MainWindow::onToolEraser()
{
    m_canvas->setToolMode(ImageCanvas::ToolMode::Eraser);
    m_toolBrushAction->setChecked(false);
    m_toolEraserAction->setChecked(true);
}

void MainWindow::onToolNone()
{
    m_canvas->setToolMode(ImageCanvas::ToolMode::None);
    m_toolBrushAction->setChecked(false);
    m_toolEraserAction->setChecked(false);
}

void MainWindow::onForegroundColorChanged(const QColor& color)
{
    m_canvas->setToolColor(color);
}
