#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QList>
#include "InputDialog.h"
#include "SliderDialog.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pendingSaveModifications(false)
{
    ui->setupUi(this);

    ui->graphicsView->hide();
    ui->statusbar->hide();
    ui->ToolBox->hide();

    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);

    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionZoomInc->setEnabled(false);
    ui->actionZoomDec->setEnabled(false);
    ui->actionZoom_Adapt->setEnabled(false);
    ui->actionSelect->setEnabled(false);
    ui->actionMove->setEnabled(false);
    ui->actionPaintBucket->setEnabled(false);
    ui->actionColorPicker->setEnabled(false);
    ui->actionBrush->setEnabled(false);
    ui->actionEraser->setEnabled(false);
    ui->actionLasso->setEnabled(false);
    ui->actionMagicWand->setEnabled(false);
    ui->actionDrawShapes->setEnabled(false);
    ui->menuEdit->setEnabled(false);
    ui->menuFilters->setEnabled(false);
    ui->menuTools->setEnabled(false);

    ui->statusbar->addPermanentWidget(&imageName);
    ui->statusbar->addPermanentWidget(&imageSize);
    ui->statusbar->addPermanentWidget(&mousePosition);

    activeImage.reset();
    painterImage.reset();
    mergeImage.reset();
    positionList.reset();

    zoomList << 1.0 << 1.5 << 2.2 << 3.3 << 4.7 << 6.8;
    zoomList << 10 << 15 << 22 << 33 << 47 << 68;
    zoomList << 100 << 150 << 220 << 330 << 470 << 680;
    zoomList << 1000 << 1500 << 2200 << 3300 << 4700 << 6800;

    QCoreApplication::setApplicationName(QString("Image Editor"));
    APP_NAME = QCoreApplication::applicationName();
    QCoreApplication::setApplicationVersion(QString("1.0.0"));
    APP_VERSION = QCoreApplication::applicationVersion();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetLayers()
{
    QPixmap temp;
    croppedImageItem->setPixmap(temp);
    rectangleRemoveItem->setRect(QRect());
    pathItem->setPath(QPainterPath());
    darkOverlayItem->setRect(pixmapItem->boundingRect());
    croppedImageItem->hide();
    rectangleRemoveItem->hide();
    pathItem->hide();
    darkOverlayItem->hide();
}

void MainWindow::resetButtons()
{
    ui->actionSelect->setEnabled(true);
    ui->actionMove->setEnabled(true);
    ui->actionPaintBucket->setEnabled(true);
    ui->actionColorPicker->setEnabled(true);
    ui->actionBrush->setEnabled(true);
    ui->actionEraser->setEnabled(true);
    ui->actionLasso->setEnabled(true);
    ui->actionMagicWand->setEnabled(true);
    ui->actionDrawShapes->setEnabled(true);

    ui->actionOpen->setEnabled(true);
}

void MainWindow::resetToolBox()
{
    ui->ToolButton->show();
    ui->ToolLabel->show();
    ui->ToolSlider->show();
    ui->ToolTitle->show();

    ui->ToolBox->hide();
}

void MainWindow::modifyPathLayer(bool append)
{
    if (positionList->empty()) {
        qPath = new QPainterPath();
        QBrush brush(Qt::DiagCrossPattern);
        brush.setColor(QColor(255,255,255));
        pathItem->setPath(*qPath);
        pathItem->setBrush(brush);
    } else {
        QPainterPath tempPath;
        tempPath.setFillRule(Qt::WindingFill);
        tempPath.moveTo(positionList->at(size_t(0)));
        for (size_t i = 1; i < positionList->size(); i++) {
            tempPath.lineTo(positionList->at(i));
        }
        tempPath.closeSubpath();
        if (append) {
            qPath->addPath(tempPath);
        } else {
            qPath = new QPainterPath(qPath->subtracted(tempPath));
        }
        QBrush brush(Qt::DiagCrossPattern);
        brush.setColor(QColor(255,255,255));
        pathItem->setPath(*qPath);
        pathItem->setBrush(brush);
    }
}

void MainWindow::setPathLayer()
{
    if (positionList->empty()) {
        qPath = new QPainterPath();
        QBrush brush(Qt::DiagCrossPattern);
        brush.setColor(QColor(255,255,255));
        pathItem->setPath(*qPath);
        pathItem->setBrush(brush);
    } else {
        qPath = new QPainterPath();
        qPath->setFillRule(Qt::WindingFill);
        qPath->moveTo(positionList->at(size_t(0)));
        for (size_t i = 1; i < positionList->size(); i++) {
            qPath->lineTo(positionList->at(i));
        }
        qPath->closeSubpath();
        QBrush brush(Qt::DiagCrossPattern);
        brush.setColor(QColor(255,255,255));
        pathItem->setPath(*qPath);
        pathItem->setBrush(brush);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString imagePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr(IMAGE_FORMATS_LIST));

    if (!imagePath.isEmpty()) {
        activeImage.reset(new Image(imagePath));

        if (activeImage->isValid()) {
            qDebug() << activeImage->getFilename();

            positionList.reset(new std::vector<QPointF>);

            scene.clear();

            commandManager.emptyUndoStack();
            commandManager.emptyRedoStack();

            scene.addRect(QRect(0, 0, activeImage->getW(), activeImage->getH()),QPen(QBrush(QColor(86,165,165)), 1));

            pixmapItem = scene.addPixmap(QPixmap::fromImage(activeImage->getQImage()));
            scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

            ui->graphicsView->setScene(&scene);
            painterImage.reset(new Image(pixmapItem->pixmap().toImage()));

            ui->graphicsView->viewport()->installEventFilter(this);
            ui->graphicsView->installEventFilter(this);

            croppedImageItem = new QGraphicsPixmapItem;
            rectangleRemoveItem = new QGraphicsRectItem();
            pathItem = new QGraphicsPathItem();
            darkOverlayItem = new QGraphicsRectItem();
            darkOverlayItem->setRect(pixmapItem->boundingRect());
            darkOverlayItem->setBrush(QBrush(QColor(0, 0, 0, 128)));
            scene.addItem(darkOverlayItem);
            scene.addItem(rectangleRemoveItem);
            scene.addItem(pathItem);
            scene.addItem(croppedImageItem);
            croppedImageItem->hide();
            rectangleRemoveItem->hide();
            pathItem->hide();
            darkOverlayItem->hide();

            ui->graphicsView->setBackgroundBrush(Qt::CrossPattern);

            ui->graphicsView->show();

            updateStatusBar();
            ui->statusbar->show();
            ui->graphicsView->fitInView(pixmapItem, Qt::KeepAspectRatio);

            ui->actionRedo->setEnabled(false);
            ui->actionUndo->setEnabled(false);
            ui->actionSave->setEnabled(false);
            ui->actionSave_as->setEnabled(true);
            ui->actionZoomInc->setEnabled(true);
            ui->actionZoomDec->setEnabled(true);
            ui->actionZoom_Adapt->setEnabled(true);
            ui->actionSelect->setEnabled(true);
            ui->actionMove->setEnabled(true);
            ui->actionPaintBucket->setEnabled(true);
            ui->actionColorPicker->setEnabled(true);
            ui->actionBrush->setEnabled(true);
            ui->actionEraser->setEnabled(true);
            ui->actionLasso->setEnabled(true);
            ui->actionMagicWand->setEnabled(true);
            ui->actionDrawShapes->setEnabled(true);
            ui->menuEdit->setEnabled(true);
            ui->menuFilters->setEnabled(true);
            ui->menuTools->setEnabled(true);
            pendingSaveModifications = false;
        } else {
            activeImage.reset();
            scene.clear();
            ui->graphicsView->hide();
            ui->statusbar->hide();

            ui->actionSave->setEnabled(false);
            ui->actionSave_as->setEnabled(false);
            ui->actionRedo->setEnabled(false);
            ui->actionUndo->setEnabled(false);
            ui->actionZoomInc->setEnabled(false);
            ui->actionZoomDec->setEnabled(false);
            ui->actionZoom_Adapt->setEnabled(false);
            ui->actionSelect->setEnabled(false);
            ui->actionMove->setEnabled(false);
            ui->actionPaintBucket->setEnabled(false);
            ui->actionColorPicker->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);
            ui->menuEdit->setEnabled(false);
            ui->menuFilters->setEnabled(false);
            ui->menuTools->setEnabled(false);

            QMessageBox::critical(this, APP_NAME, "The image is not Valid.", QMessageBox::Ok);
        }
    }
}

void MainWindow::zoomUpdate(bool increment)
{
    if (activeImage != nullptr) {
        double zoom = ui->graphicsView->transform().m11() * 100;

        if (increment) {
            for (double z: zoomList) {
                if ((z - z / 10) > zoom) {
                    zoom = z;
                    break;
                }
            }
        } else {
            for (int i = zoomList.count() - 1; i > 0; i--) {
                double zoomIt = zoomList[i];

                if ((zoomIt + zoomIt / 10) < zoom) {
                    zoom = zoomIt;
                    break;
                }
            }
        }

        qDebug() << "zoom value " << zoom;
        ui->graphicsView->setTransform(QTransform::fromScale(zoom / 100, zoom / 100));
    }
}

void MainWindow::updateStatusBar()
{
    if (activeImage != nullptr) {

        imageName.setText(activeImage->getFilename());
        imageSize.setText(QString("%1x%2")
                              .arg(activeImage->getW())
                              .arg(activeImage->getH()));
        mousePosition.setText(QString("x: %1, y: %2")
                                  .arg(int(currentMousePosition.x()))
                                  .arg(int(currentMousePosition.y())));
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (activeImage != nullptr) {

        activeImage->save(activeImage->getPath());
        ui->actionSave->setEnabled(false);
        pendingSaveModifications = false;
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    if(activeImage!= nullptr){
        QString filename = QFileDialog::getSaveFileName(this,
                                                        "",
                                                        activeImage->getFilename(),
                                                        tr(IMAGE_FORMATS_LIST));

        if(!filename.isEmpty()){
            if(activeImage->save(filename)) {
                activeImage->setPath(filename);
                updateStatusBar();

                pendingSaveModifications = false;
                ui->actionSave->setEnabled(false);
            }
            else{
                QMessageBox::critical(this, APP_NAME,
                                      "The path is not valid. Please check the image format.",
                                      QMessageBox::Ok);
            }
        }
    }
}

void MainWindow::on_actionZoom_Adapt_triggered()
{
    if (activeImage != nullptr) {
        ui->graphicsView->fitInView(pixmapItem, Qt::KeepAspectRatio);
    }
}

void MainWindow::on_actionZoomInc_triggered()
{
    if (activeImage != nullptr) {
        zoomUpdate(true);
        qDebug() << "zoom In";
    }
}

void MainWindow::on_actionZoomDec_triggered() {

    if (activeImage != nullptr) {
        zoomUpdate(false);
        qDebug() << "zoom out";
    }
}

void MainWindow::on_actionRotate_Clockwise_triggered()
{
    if (activeImage != nullptr) {
        std::shared_ptr<ICommand> c1(new rotateClockwiseCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "rotate clockwise";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());
        on_actionZoom_Adapt_triggered();
        updateStatusBar();

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionRotate_anticlockwise_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new rotateAntiClockwiseCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "rotate anticlockwise";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());
        on_actionZoom_Adapt_triggered();
        updateStatusBar();

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionFlip_x_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new flipXCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "flip x";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionFlip_y_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new flipYCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "flip y";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionUndo_triggered()
{
    if(activeImage!= nullptr && !commandManager.isUndoStackEmpty()) {
        commandManager.undo();
        qDebug() << "undo";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        updateStatusBar();
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

        pendingSaveModifications = true;
        ui->actionRedo->setEnabled(true);
        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);

        if(commandManager.isUndoStackEmpty())
            ui->actionUndo->setEnabled(false);

        if(magicWandFlag)
            setPathLayer();
    }
}

void MainWindow::on_actionRedo_triggered()
{
    if(activeImage!= nullptr && !commandManager.isRedoStackEmpty()) {
        commandManager.redo();
        qDebug() << "redo";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        updateStatusBar();
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

        pendingSaveModifications = true;
        ui->actionUndo->setEnabled(true);
        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);

        if(commandManager.isRedoStackEmpty())
            ui->actionRedo->setEnabled(false);

        if(magicWandFlag)
            setPathLayer();
    }
}

void MainWindow::on_actionGrayscale_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new grayScaleCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "grayscale";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionBlur_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new blurCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "blur";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionSharpen_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new sharpenCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "sharpen";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionEmboss_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new embossCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "emboss";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::on_actionEdge_detect_triggered()
{
    if(activeImage!= nullptr) {
        std::shared_ptr<ICommand> c1(new edgeDetectCommand(*activeImage));
        commandManager.execute(c1);
        qDebug() << "edgedetect";
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (pendingSaveModifications && !ui->graphicsView->isHidden()) {
        QString title = APP_NAME;
        QString message = "<p>Any modifications not saved will be lost. Are you "
                          "sure to close Image Editor?</p>";

        int status = QMessageBox::question(this, title, message, QMessageBox::Ok, QMessageBox::Cancel);

        if (status == QMessageBox::Ok) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_Image_Editor_triggered()
{
    QString title = APP_NAME;
    QString msg = QString("<h2>%1</h2>")
                      .arg(title);
    msg += QString("<p>"
                   "A functional image editor.</p>");
    msg += QString("<p><b>Author: </b><a>Jakub Borawski</a></p>");
    msg += QString("<p><b>Version: </b>%1")
               .arg(APP_VERSION);

    QMessageBox::about(this, title, msg);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionContrast_triggered()
{
    if (activeImage != nullptr) {
        bool ok = false;
        QList<QString> field = {"Contrast"};
        QList<int> input = InputDialog::getFields(this, field, -100, 100, 10, &ok);

        if (ok) {
            int inputValue = input[0];
            std::shared_ptr<ICommand>c1(new contrastCommand(*activeImage, inputValue));
            commandManager.execute(c1);
            qDebug() << "contrast";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
        }
    }
}

void MainWindow::on_actionBrightness_triggered()
{
    if(activeImage!=nullptr){
        bool ok = false;
        QList<QString> field = {"Brightness"};
        QList<int> input = InputDialog::getFields(this,
                                                  field,
                                                  -100, 100, 10, &ok);

        if(ok){
            int inputValue = input[0];
            std::shared_ptr<ICommand>c1(new brightnessCommand(*activeImage, inputValue));
            commandManager.execute(c1);
            qDebug() << "brightness";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
        }
    }
}

void MainWindow::on_actionColormask_triggered()
{
    if(activeImage!=nullptr){
        bool ok = false;
        QList<QString> fields = {"Red", "Green", "Blue"};
        QList<int> values = InputDialog::getFields(this, fields, 1, 255, 1, &ok);

        if(ok){
            int r = values[0];
            int g = values[1];
            int b = values[2];

            std::shared_ptr<ICommand> c(new colorMaskCommand(*activeImage, r, g, b));
            commandManager.execute(c);
            qDebug() << "colormask";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
        }
    }
}

void MainWindow::on_actionScale_triggered()
{
    if(activeImage!=nullptr){
        bool ok = false;
        QList<QString> fields = {"Width", "Height"};
        QList<int> values = InputDialog::getFields(this, fields, 1, 10000, 100, &ok);


        if(ok){
            int w = values[0];
            int h = values[1];

            std::shared_ptr<ICommand> c(new scaleCommand(*activeImage, w, h));
            commandManager.execute(c);
            qDebug() << "scale";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
            scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());
            on_actionZoom_Adapt_triggered();

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
            updateStatusBar();
        }
    }
}

void MainWindow::on_actionAutoCrop_triggered()
{
    if (activeImage != nullptr) {
        bool ok = false;
        QList<QString> field = {"Crop Black or White Border?"};
        QList<int> values = InputDialog::getFields(this, field, 0, 1, 1, &ok);

        if (ok) {
            int is_black = values[0];

            std::shared_ptr<ICommand> c(new autoCropCommand(*activeImage, is_black));
            commandManager.execute(c);
            qDebug() << "autocrop";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
            scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());
            on_actionZoom_Adapt_triggered();

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
            updateStatusBar();
        }
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        currentMousePosition = ui->graphicsView->mapToScene(mEvent->pos());
        updateStatusBar();
    }

    if (event->type() == QEvent::MouseButtonRelease && selectFlag && !moveFlag)
    {
        qDebug("EVENT FILTER - MOUSEBUTTON_RELEASE");

        if(!ui->graphicsView->rubberBandRect().isEmpty())
        {
            QPixmap croppedPixmap = pixmapItem->pixmap().copy(ui->graphicsView->mapToScene(ui->graphicsView->rubberBandRect()).boundingRect().toRect());

            if(croppedPixmap.isNull())
                return QObject::eventFilter(object, event);

            croppedImageItem->setPixmap(croppedPixmap);

            croppedImageItem->setPos(ui->graphicsView->mapToScene(ui->graphicsView->rubberBandRect().topLeft()));

            rectangleRemoveItem->setRect(ui->graphicsView->mapToScene(ui->graphicsView->rubberBandRect()).boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));

            croppedImageItem->show();

            darkOverlayItem->show();
        }
    }

    if (event->type() == QEvent::KeyPress && selectFlag && !moveFlag) {
        QKeyEvent *kEvent = static_cast<QKeyEvent *>(event);
        if (kEvent->key() == Qt::Key_Delete) {
            if (!croppedImageItem->pixmap().isNull()) {
                on_actionDeleteRect_triggered();
                resetLayers();
                croppedImageItem->show();
                rectangleRemoveItem->show();
                darkOverlayItem->show();
            }
        }
    }

    if (event->type() == QEvent::MouseButtonPress && brushFlag && !isMovingFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (mEvent->modifiers() != Qt::CTRL) {
            isMovingFlag = true;

            bufferPix = pixmapItem->pixmap();
            qPainter = new QPainter(&bufferPix);
            qPainter->setRenderHint(QPainter::Antialiasing);
            QPen *pen = new QPen(currentColor);
            pen->setStyle(Qt::SolidLine);
            pen->setWidth(brushSize);
            pen->setCapStyle(Qt::RoundCap);
            pen->setJoinStyle(Qt::RoundJoin);
            qPainter->setPen(*pen);
        }
    }

    if (event->type() == QEvent::MouseMove && brushFlag && isMovingFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
        QPoint imgPos(int(imgPosF.x()), int(imgPosF.y()));
        qPainter->drawPoint(imgPos);
        pixmapItem->setPixmap(bufferPix);
    }

    if (event->type() == QEvent::MouseButtonRelease && brushFlag && isMovingFlag && !selectFlag && !moveFlag) {
        isMovingFlag = false;
        qPainter->end();
        on_actionPixmapToImage_triggered();
    }

    if (event->type() == QEvent::MouseButtonPress && eraserFlag && !isMovingFlag && !brushFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (mEvent->modifiers() != Qt::CTRL) {
            isMovingFlag = true;

            bufferPix = pixmapItem->pixmap();
            qPainter = new QPainter(&bufferPix);
            qPainter->setRenderHint(QPainter::Antialiasing);
            QPen *pen = new QPen(QColor(0,38,38,150));

            pen->setStyle(Qt::SolidLine);
            pen->setWidth(eraserSize);
            pen->setCapStyle(Qt::RoundCap);
            pen->setJoinStyle(Qt::RoundJoin);
            qPainter->setPen(*pen);
        }
    }

    if (event->type() == QEvent::MouseMove && eraserFlag && isMovingFlag && !brushFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
        QPoint imgPos(int(imgPosF.x()), int(imgPosF.y()));
        qPainter->drawPoint(imgPos);
        pixmapItem->setPixmap(bufferPix);
    }

    if (event->type() == QEvent::MouseButtonRelease && eraserFlag && isMovingFlag && !brushFlag && !selectFlag && !moveFlag) {
        isMovingFlag = false;
        qPainter->end();
        on_actionPixmapToImage_triggered();
    }

    if (event->type() == QEvent::MouseButtonPress && lassoFlag && !isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (mEvent->modifiers() != Qt::CTRL && mEvent->modifiers() != Qt::SHIFT) {
            isMovingFlag = true;

            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
            qPath = new QPainterPath();
            qPath->moveTo(imgPosF);
        } else if (mEvent->modifiers() == Qt::SHIFT) {
            isMovingFlag = true;
            isMultipleLasso = true;
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
            tempQPath = QPainterPath();
            tempQPath.moveTo(imgPosF);
        }
    }

    if (event->type() == QEvent::MouseMove && lassoFlag && isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (isMultipleLasso) {
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
            tempQPath.lineTo(imgPosF);
        } else {
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
            qPath->lineTo(imgPosF);
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && lassoFlag && isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag) {
        if (isMultipleLasso) {
            isMovingFlag = false;
            isMultipleLasso = false;

            tempQPath.closeSubpath();
            qPath->addPath(tempQPath);
            QBrush brush(Qt::DiagCrossPattern);
            brush.setColor(QColor(255,255,255));
            pathItem->setBrush(brush);
            pathItem->setPath(*qPath);

            QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

            if (croppedPixmap.isNull()) {
                qDebug("Lasso - croppedPixmap is NULL");
                return QObject::eventFilter(object, event);
            }

            croppedImageItem->setPixmap(croppedPixmap);
            croppedImageItem->setPos(qPath->boundingRect().topLeft());

            rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
        } else {
            isMovingFlag = false;

            qPath->closeSubpath();
            QBrush brush(Qt::DiagCrossPattern);
            brush.setColor(QColor(255,255,255));
            pathItem->setBrush(brush);
            pathItem->setPath(*qPath);

            QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

            if (croppedPixmap.isNull()) {
                qDebug("Lasso - croppedPixmap is NULL");
                return QObject::eventFilter(object, event);
            }

            croppedImageItem->setPixmap(croppedPixmap);
            croppedImageItem->setPos(qPath->boundingRect().topLeft());

            rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
        }
    }

    if (event->type() == QEvent::KeyPress && lassoFlag && !isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag) {
        QKeyEvent *kEvent = static_cast<QKeyEvent *>(event);
        if (!pathItem->path().isEmpty()) {

            if (kEvent->key() == Qt::Key_Delete) {
                eraserFlag = true;

                bufferPix = pixmapItem->pixmap();
                qPainter = new QPainter(&bufferPix);

                qPainter->fillPath(*qPath, QBrush(QColor(0,38,38,150)));
                pixmapItem->setPixmap(bufferPix);
                qPainter->end();
                on_actionPixmapToImage_triggered();

                resetLayers();
                pathItem->show();

                eraserFlag = false;
            }

            if (kEvent->key() == Qt::Key_I) {
                QPainterPath tempPath;
                tempPath.moveTo(QPointF(0, 0));
                tempPath.lineTo(QPointF(activeImage->getW(), 0));
                tempPath.lineTo(QPointF(activeImage->getW(), activeImage->getH()));
                tempPath.lineTo(QPointF(0, activeImage->getH()));
                tempPath.closeSubpath();
                qPath = new QPainterPath(tempPath.subtracted(*qPath));
                QBrush brush(Qt::DiagCrossPattern);
                brush.setColor(QColor(255,255,255));
                pathItem->setPath(*qPath);
                pathItem->setBrush(brush);

                QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

                if (croppedPixmap.isNull()) {
                    qDebug("Lasso - croppedPixmap is NULL");
                    return QObject::eventFilter(object, event);
                }

                croppedImageItem->setPixmap(croppedPixmap);
                croppedImageItem->setPos(qPath->boundingRect().topLeft());

                rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
                rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
            }
        }
    }

    if (event->type() == QEvent::MouseButtonPress && magicWandFlag && !isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag && !lassoFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);;
        if (magicWandWasTriggered) {
            magicWandWasTriggered = false;
            return QObject::eventFilter(object, event);
        }
        if (mEvent->modifiers() != Qt::CTRL && mEvent->modifiers() != Qt::SHIFT) {
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());

            magicWand(imgPosF);

            magicWandWasTriggered = true;

            setPathLayer();

            QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

            if (croppedPixmap.isNull()) {
                qDebug("MagicWand - croppedPixmap is NULL");
                return QObject::eventFilter(object, event);
            }

            croppedImageItem->setPixmap(croppedPixmap);
            croppedImageItem->setPos(qPath->boundingRect().topLeft());

            rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
        } else if (mEvent->modifiers() != Qt::CTRL && mEvent->modifiers() == Qt::SHIFT && !isMagicRemove) {
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());

            magicWand(imgPosF);

            magicWandWasTriggered = true;

            modifyPathLayer(true);

            QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

            if (croppedPixmap.isNull()) {
                qDebug("MagicWand - croppedPixmap is NULL");
                return QObject::eventFilter(object, event);
            }

            croppedImageItem->setPixmap(croppedPixmap);
            croppedImageItem->setPos(qPath->boundingRect().topLeft());

            rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
        } else if (mEvent->modifiers() != Qt::CTRL && mEvent->modifiers() == Qt::SHIFT && isMagicRemove) {
            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());

            magicWand(imgPosF);

            magicWandWasTriggered = true;

            modifyPathLayer(false);

            QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

            if (croppedPixmap.isNull()) {
                qDebug("MagicWand - croppedPixmap is NULL");
                return QObject::eventFilter(object, event);
            }

            croppedImageItem->setPixmap(croppedPixmap);
            croppedImageItem->setPos(qPath->boundingRect().topLeft());

            rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
            rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
        }
    }

    if (event->type() == QEvent::KeyPress && magicWandFlag && !isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag && !lassoFlag) {
        QKeyEvent *kEvent = static_cast<QKeyEvent *>(event);
        if (!pathItem->path().isEmpty()) {
            if (kEvent->key() == Qt::Key_Delete) {
                eraserFlag = true;

                bufferPix = pixmapItem->pixmap();
                qPainter = new QPainter(&bufferPix);

                qPainter->fillPath(*qPath, QBrush(QColor(0,38,38,150)));
                pixmapItem->setPixmap(bufferPix);
                qPainter->end();
                on_actionPixmapToImage_triggered();

                resetLayers();
                pathItem->show();

                eraserFlag = false;
            }

            if (kEvent->key() == Qt::Key_I) {
                QPainterPath tempPath;
                tempPath.moveTo(QPointF(0, 0));
                tempPath.lineTo(QPointF(activeImage->getW(), 0));
                tempPath.lineTo(QPointF(activeImage->getW(), activeImage->getH()));
                tempPath.lineTo(QPointF(0, activeImage->getH()));
                tempPath.closeSubpath();

                qPath = new QPainterPath(tempPath.subtracted(*qPath));
                QBrush brush(Qt::DiagCrossPattern);
                brush.setColor(QColor(255,255,255));
                pathItem->setPath(*qPath);
                pathItem->setBrush(brush);

                QPixmap croppedPixmap = pixmapItem->pixmap().copy(qPath->boundingRect().toRect());

                if (croppedPixmap.isNull()) {
                    qDebug("MagicWand - croppedPixmap is NULL");
                    return QObject::eventFilter(object, event);
                }

                croppedImageItem->setPixmap(croppedPixmap);
                croppedImageItem->setPos(qPath->boundingRect().topLeft());

                rectangleRemoveItem->setRect(qPath->boundingRect().toRect());
                rectangleRemoveItem->setBrush(QBrush(QColor(0, 38, 38, 255)));
            }
        }
    }

    if (event->type() == QEvent::MouseButtonPress && drawShapesFlag && !isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag && !lassoFlag && !magicWandFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (mEvent->modifiers() != Qt::CTRL) {
            if (!ui->actionOptionRectangle->isChecked() && !ui->actionOptionTriangle->isChecked() && !ui->actionOptionCircle->isChecked()) {
                qDebug("No Shape Option Selected");
                return QObject::eventFilter(object, event);
            }
            isMovingFlag = true;

            QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());

            bufferPix = pixmapItem->pixmap();
            qPainter = new QPainter(&bufferPix);
            qPainter->setRenderHint(QPainter::Antialiasing);
            QPen *pen = new QPen(QColor(255,0,0,255));
            pen->setStyle(Qt::SolidLine);
            pen->setWidth(30);
            pen->setCapStyle(Qt::RoundCap);
            pen->setJoinStyle(Qt::RoundJoin);
            qPainter->setPen(*pen);

            qPainter->drawPoint(imgPosF);
            lastMouseLocation = imgPosF;
            qPainter->end();

            croppedImageItem->setPixmap(bufferPix);
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && drawShapesFlag && isMovingFlag && !eraserFlag && !brushFlag && !selectFlag && !moveFlag && !lassoFlag && !magicWandFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        isMovingFlag = false;

        QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());

        bufferPix = pixmapItem->pixmap();
        qPainter = new QPainter(&bufferPix);
        qPainter->setRenderHint(QPainter::Antialiasing);

        if (ui->actionOptionRectangle->isChecked()) {
            qPainter->fillRect(QRect(QPoint(lastMouseLocation.x(), lastMouseLocation.y()), QPoint(imgPosF.x(), imgPosF.y())), QBrush(currentColor));
        } else if (ui->actionOptionCircle->isChecked()) {
            qPainter->setBrush(currentColor);
            int radius = abs(lastMouseLocation.manhattanLength() - imgPosF.manhattanLength());
            qDebug("Radius: %d", radius);
            qPainter->drawEllipse(lastMouseLocation, radius, radius);
        } else if (ui->actionOptionTriangle->isChecked()) {
            qPath = new QPainterPath();

            double differenceX = abs(lastMouseLocation.x() - imgPosF.x()) * 2;
            QPointF leftPoint(imgPosF.x() - differenceX, imgPosF.y());
            qPath->moveTo(lastMouseLocation);
            qPath->lineTo(imgPosF);
            qPath->lineTo(leftPoint);
            qPath->closeSubpath();
            qPainter->fillPath(*qPath, QBrush(currentColor));
        }

        qPainter->end();

        croppedImageItem->setPixmap(QPixmap());
        pixmapItem->setPixmap(bufferPix);
        on_actionPixmapToImage_triggered();

    }

    if (event->type() == QEvent::MouseButtonPress && moveFlag && !isMovingFlag && !selectFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if (croppedImageItem->isUnderMouse()) {
            QPointF imgPos = ui->graphicsView->mapToScene(mEvent->pos());
            lastMouseLocation = imgPos;
            isMovingFlag = true;
        }
    }

    if (event->type() == QEvent::MouseMove && moveFlag && isMovingFlag && !selectFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        QPointF imgPos = ui->graphicsView->mapToScene(mEvent->pos());
        croppedImageItem->moveBy((imgPos.x()-lastMouseLocation.x()), imgPos.y()-lastMouseLocation.y());
        QPointF imgPosNew = ui->graphicsView->mapToScene(mEvent->pos());
        lastMouseLocation = imgPosNew;
    }

    if (event->type() == QEvent::MouseButtonRelease && moveFlag && isMovingFlag && !selectFlag) {
        isMovingFlag = false;
        ui->actionMove->setChecked(false);
        on_actionMove_triggered();
    }

    if (event->type() == QEvent::MouseButtonPress && paintBucketFlag && !selectFlag && !moveFlag && !isMovingFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        QPointF imgPosF = ui->graphicsView->mapToScene(mEvent->pos());
        paintBucket(imgPosF);
        paintBucketFlag = false;
        ui->actionPaintBucket->setChecked(paintBucketFlag);
        on_actionPaintBucket_triggered();
    }

    if (event->type() == QEvent::MouseButtonPress && !selectFlag && !moveFlag) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if ( mEvent->modifiers() == Qt::CTRL) {
            ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && !selectFlag && !moveFlag) {
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }

    if (event->type() == QEvent::Wheel) {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);

        if (mEvent->modifiers() != Qt::CTRL && mEvent->modifiers() != Qt::SHIFT) {
            qDebug("WheelEvent");
            ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            QWheelEvent *scrollevent = static_cast<QWheelEvent *>(event);
            if (scrollevent->angleDelta().y() > 0) {
                qDebug() << "ZOOM IN MOUSE";
                on_actionZoomInc_triggered();
            } else if (scrollevent->angleDelta().y() < 0) {
                qDebug() << "ZOOM OUT MOUSE";
                on_actionZoomDec_triggered();
            }
        }
    }

    return QObject::eventFilter(object, event);
}

void MainWindow::on_actionMergeLayers_triggered()
{
    if (activeImage != nullptr) {
        QString title = APP_NAME;
        QString message = "<p>Are you sure you want to merge layers?</p>";
        int status = QMessageBox::question(this, title, message, QMessageBox::Ok, QMessageBox::Cancel);

        if (status == QMessageBox::Ok) {
            if (!ui->actionCopy_Selection->isChecked()) {
                on_actionDeleteRect_triggered();
            }
            mergeImage.reset(new Image(croppedImageItem->pixmap().toImage()));
            std::shared_ptr<ICommand> c(new mergeLayersCommand(*activeImage, *mergeImage, croppedImageItem->pos().x(),croppedImageItem->pos().y()));
            commandManager.execute(c);
            qDebug("MergeLayers");
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
            scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

            resetLayers();

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
            updateStatusBar();
        } else {
            resetLayers();
            ui->actionRedo->setEnabled(false);
            return;
        }
    }
}

void MainWindow::on_actionDeleteRect_triggered()
{
    if (activeImage != nullptr) {
        int rectPosX = rectangleRemoveItem->rect().x();
        int rectPosY = rectangleRemoveItem->rect().y();
        int rectW = rectangleRemoveItem->rect().width();
        int rectH = rectangleRemoveItem->rect().height();
        std::shared_ptr<ICommand> c(new deleteRectCommand(*activeImage, rectPosX, rectPosY, rectW, rectH));
        commandManager.execute(c);
        qDebug("DeleteRect");
        qDebug("rectW,H: %d, %d", croppedImageItem->pixmap().width(), croppedImageItem->pixmap().height());
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
        updateStatusBar();
    }
}

void MainWindow::on_actionSaturation_triggered()
{
    if(activeImage != nullptr){
        bool ok = false;
        QList<QString> fields = {"Red", "Green", "Blue"};
        QList<int> values = InputDialog::getFields(this, fields, 1, 255, 1, &ok);

        if(ok){
            int r = values[0];
            int g = values[1];
            int b = values[2];

            std::shared_ptr<ICommand> c(new colorSaturationCommand(*activeImage, r, g, b));
            commandManager.execute(c);
            qDebug() << "colorSaturation";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
        }
    }
}

void MainWindow::on_actionSelect_triggered()
{
    qDebug("ActionSelect");

    selectTool(Tools(SELECT));

    selectFlag = ui->actionSelect->isChecked();

    if (selectFlag) {
        qDebug() << "Select - True";
        croppedImageItem->show();
        rectangleRemoveItem->show();
        darkOverlayItem->show();
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    } else if (!selectFlag && fromSelectFlag) {
        qDebug() << "Select - (fromSelectFlag) False";
        fromSelectFlag = false;
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        croppedImageItem->hide();
        rectangleRemoveItem->hide();
        darkOverlayItem->hide();

        resetButtons();

        ui->actionSelect->setEnabled(false);
        ui->actionPaintBucket->setEnabled(false);
        ui->actionBrush->setEnabled(false);
        ui->actionEraser->setEnabled(false);
        ui->actionLasso->setEnabled(false);
        ui->actionMagicWand->setEnabled(false);

        ui->actionOpen->setEnabled(false);

    } else if (!selectFlag) {
        qDebug() << "Select - False";
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        resetLayers();
        resetButtons();
    }
}

void MainWindow::on_actionMove_triggered()
{
    qDebug("ActionMove");
    if (croppedImageItem->pixmap().isNull()) {
        qDebug("Move - CroppedArea Empty");
        ui->actionMove->setChecked(false);
        return;
    }

    selectTool(Tools(MOVE));

    moveFlag = ui->actionMove->isChecked();

    if (moveFlag) {
        qDebug() << "Move - True";
        ui->actionCopy_Selection->setEnabled(false);
        croppedImageItem->show();
        if (!ui->actionCopy_Selection->isChecked()) {
            rectangleRemoveItem->show();
        }
        darkOverlayItem->show();

    } else if (!moveFlag) {
        qDebug() << "Move - False";
        ui->actionCopy_Selection->setEnabled(true);
        croppedImageItem->hide();
        rectangleRemoveItem->hide();
        darkOverlayItem->hide();
        on_actionMergeLayers_triggered();
        resetLayers();
        resetButtons();
    }
}

void MainWindow::on_actionPaintBucket_triggered()
{
    qDebug("ActionPaintBucket");

    selectTool(Tools(PAINT_BUCKET));

    paintBucketFlag = ui->actionPaintBucket->isChecked();

    if (paintBucketFlag) {
        qDebug() << "PaintBucket - True";
    } else if (!paintBucketFlag) {
        qDebug() << "PaintBucket - False";
        resetLayers();
        resetButtons();
    }
}

void MainWindow::paintBucket(QPointF pos)
{
    if (activeImage != nullptr) {
        std::shared_ptr<ICommand> c(new paintBucketCommand(*activeImage, currentColor, pos));
        commandManager.execute(c);
        qDebug("paintBucket");
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
        updateStatusBar();
    }
}

void MainWindow::on_actionColorPicker_triggered()
{
    QColor tempColor = colorDialog.getColor();
    if (tempColor.isValid()) {
        currentColor = tempColor;
    }
}

void MainWindow::on_actionBrush_triggered()
{
    qDebug("ActionBrush");

    selectTool(Tools(BRUSH));

    brushFlag = ui->actionBrush->isChecked();

    if (brushFlag) {
        qDebug() << "Brush - True";

        ui->ToolBox->show();
        ui->ToolTitle->setText((QString("Brush Size:")));
        ui->ToolLabel->setText((QString("%1").arg(brushSize)));
        ui->ToolSlider->setMaximum(activeImage->getW()/2);
        ui->ToolSlider->setPageStep(activeImage->getW()*0.1);
        ui->ToolSlider->setValue(brushSize);
        ui->ToolButton->hide();

    } else if (!brushFlag) {
        qDebug() << "Brush - False";
        resetLayers();
        resetButtons();
        resetToolBox();
    }
}

void MainWindow::selectTool(Tools tool)
{
    switch (tool) {
        case SELECT:
            ui->actionPaintBucket->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;
        case MOVE:
            if (ui->actionSelect->isChecked()) {
                ui->actionSelect->setChecked(false);
                fromSelectFlag = true;
                on_actionSelect_triggered();
            } else if (ui->actionLasso->isChecked()) {
                ui->actionLasso->setChecked(false);
                fromSelectFlag = true;
                on_actionLasso_triggered();
            } else if (ui->actionMagicWand->isChecked()) {
                ui->actionMagicWand->setChecked(false);
                fromSelectFlag = true;
                on_actionMagicWand_triggered();
            } else {
                ui->actionSelect->setEnabled(false);
                ui->actionPaintBucket->setEnabled(false);
                ui->actionBrush->setEnabled(false);
                ui->actionEraser->setEnabled(false);
                ui->actionLasso->setEnabled(false);
                ui->actionMagicWand->setEnabled(false);
                ui->actionDrawShapes->setEnabled(false);

                ui->actionOpen->setEnabled(false);
            }

            break;
        case PAINT_BUCKET:
            if (ui->actionLasso->isChecked()) {
                ui->actionLasso->setChecked(false);
                toPaintBucketFlag = true;
                on_actionLasso_triggered();
                ui->actionPaintBucket->setChecked(false);
            } else if (ui->actionMagicWand->isChecked()) {
                ui->actionMagicWand->setChecked(false);
                toPaintBucketFlag = true;
                on_actionMagicWand_triggered();
                ui->actionPaintBucket->setChecked(false);
            } else {
                ui->actionSelect->setEnabled(false);
                ui->actionMove->setEnabled(false);
                ui->actionBrush->setEnabled(false);
                ui->actionEraser->setEnabled(false);
                ui->actionLasso->setEnabled(false);
                ui->actionMagicWand->setEnabled(false);
                ui->actionDrawShapes->setEnabled(false);

                ui->actionOpen->setEnabled(false);
            }

            break;
        case BRUSH:
            ui->actionSelect->setEnabled(false);
            ui->actionMove->setEnabled(false);
            ui->actionPaintBucket->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;

        case ERASER:
            ui->actionSelect->setEnabled(false);
            ui->actionMove->setEnabled(false);
            ui->actionPaintBucket->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;

        case LASSO:
            ui->actionSelect->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;

        case MAGICWAND:
            ui->actionSelect->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionDrawShapes->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;

        case DRAWSHAPES:
            ui->actionSelect->setEnabled(false);
            ui->actionMove->setEnabled(false);
            ui->actionPaintBucket->setEnabled(false);
            ui->actionColorPicker->setEnabled(false);
            ui->actionBrush->setEnabled(false);
            ui->actionEraser->setEnabled(false);
            ui->actionLasso->setEnabled(false);
            ui->actionMagicWand->setEnabled(false);

            ui->actionOpen->setEnabled(false);

            break;
    }
}

void MainWindow::on_actionPixmapToImage_triggered()
{
    if (activeImage != nullptr) {
        painterImage.reset(new Image(pixmapItem->pixmap().toImage()));
        std::shared_ptr<ICommand> c(new pixmapToImageCommand(*activeImage, *painterImage, eraserFlag));
        commandManager.execute(c);
        qDebug("PixmapToImage");
        activeImage->updateBuffer();
        pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));
        scene.setSceneRect(0, 0, activeImage->getW(), activeImage->getH());

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
        updateStatusBar();
    }
}

void MainWindow::on_actionToolsBrushOptions_triggered()
{
    bool ok = false;
    QList<QString> field = {"Size"};
    QList<int> input = InputDialog::getFields(this, field, 1, activeImage->getW()/2, 5, &ok);
    if (ok) {
        brushSize = input[0];
    }
}

void MainWindow::on_actionEraser_triggered()
{
    qDebug("ActionEraser");

    selectTool(ERASER);

    eraserFlag = ui->actionEraser->isChecked();

    if (eraserFlag) {
        qDebug("Eraser - True");

        ui->ToolBox->show();
        ui->ToolTitle->setText((QString("Eraser Size:")));
        ui->ToolLabel->setText((QString("%1").arg(eraserSize)));
        ui->ToolSlider->setMaximum(activeImage->getW()/2);
        ui->ToolSlider->setPageStep(activeImage->getW()*0.1);
        ui->ToolSlider->setValue(eraserSize);
        ui->ToolButton->hide();

    } else if (!eraserFlag) {
        qDebug("Eraser - False");
        resetLayers();
        resetButtons();
        resetToolBox();
    }
}

void MainWindow::on_actionGaussianBlur_triggered()
{
    if(activeImage!= nullptr) {
        bool ok = false;
        QList<QString> field = {"Radius"};
        QList<int> input = InputDialog::getFields(this, field, 1, 35, 1, &ok);

        if (ok) {
            int radius = input[0];
            std::shared_ptr<ICommand> c(new blurGaussianCommand(*activeImage, radius));
            commandManager.execute(c);
            qDebug() << "gaussian blur";
            activeImage->updateBuffer();
            pixmapItem->setPixmap(QPixmap::fromImage(activeImage->getQImage()));

            pendingSaveModifications = true;
            ui->actionSave->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionRedo->setEnabled(false);
            }
    }
}

void MainWindow::on_actionToolsEraserOptions_triggered()
{
    bool ok = false;
    QList<QString> field = {"Size"};
    QList<int> input = InputDialog::getFields(this, field, 1, activeImage->getW()/2, 5, &ok);
    if (ok) {
        eraserSize = input[0];
    }
}

void MainWindow::on_actionLasso_triggered()
{
    qDebug("ActionLasso");

    selectTool(LASSO);

    lassoFlag = ui->actionLasso->isChecked();

    if (lassoFlag) {
        qDebug("Lasso - True");
        pathItem->show();
    } else if (!lassoFlag && fromSelectFlag) {
        qDebug() << "Lasso - (fromSelectFlag) False";
        fromSelectFlag = false;
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);

        pathItem->hide();

        pathItem->setPath(QPainterPath());

        resetButtons();

        ui->actionSelect->setEnabled(false);
        ui->actionPaintBucket->setEnabled(false);
        ui->actionBrush->setEnabled(false);
        ui->actionEraser->setEnabled(false);
        ui->actionLasso->setEnabled(false);
        ui->actionMagicWand->setEnabled(false);

        ui->actionOpen->setEnabled(false);

    } else if (!lassoFlag && toPaintBucketFlag) {
        qDebug() << "Lasso - (toPaintBucketFlag) False";
        toPaintBucketFlag = false;

        bufferPix = pixmapItem->pixmap();
        qPainter = new QPainter(&bufferPix);

        qPainter->fillPath(*qPath, QBrush(currentColor));
        pixmapItem->setPixmap(bufferPix);
        qPainter->end();
        on_actionPixmapToImage_triggered();

        resetLayers();

    } else if (!lassoFlag) {
        qDebug("Lasso - False");
        resetLayers();
        resetButtons();
    }
}

void MainWindow::on_actionMagicWand_triggered()
{
    qDebug("ActionMagicWand");

    selectTool(MAGICWAND);

    magicWandFlag = ui->actionMagicWand->isChecked();

    if (magicWandFlag) {
        qDebug("MagicWand - True");
        setPathLayer();
        pathItem->show();

        ui->ToolBox->show();
        ui->ToolTitle->setText((QString("Tolerance:")));
        ui->ToolLabel->setText((QString("%1").arg(currentTolerance)));
        ui->ToolButton->setText(isMagicRemove ? QString("Remove") : QString("Append"));
        ui->ToolSlider->setValue(currentTolerance);
        ui->ToolSlider->setMaximum(MAX_TOLERANCE);
        ui->ToolSlider->setPageStep(2);

    } else if (!magicWandFlag && fromSelectFlag) {
        qDebug() << "MagicWand - (fromSelectFlag) False";
        fromSelectFlag = false;
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);

        pathItem->hide();

        pathItem->setPath(QPainterPath());

        resetButtons();

        ui->actionSelect->setEnabled(false);
        ui->actionPaintBucket->setEnabled(false);
        ui->actionBrush->setEnabled(false);
        ui->actionEraser->setEnabled(false);
        ui->actionLasso->setEnabled(false);
        ui->actionMagicWand->setEnabled(false);

        ui->actionOpen->setEnabled(false);

        resetToolBox();

    } else if (!magicWandFlag && toPaintBucketFlag) {
        qDebug() << "MagicWand - (toPaintBucketFlag) False";
        toPaintBucketFlag = false;

        bufferPix = pixmapItem->pixmap();
        qPainter = new QPainter(&bufferPix);

        qPainter->fillPath(*qPath, QBrush(currentColor));
        pixmapItem->setPixmap(bufferPix);
        qPainter->end();
        on_actionPixmapToImage_triggered();

        resetToolBox();

        resetLayers();
    } else if (!magicWandFlag) {
        qDebug("MagicWand - False");

        resetToolBox();

        resetLayers();
        resetButtons();
    }
}

void MainWindow::magicWand(QPointF pos)
{
    if (activeImage != nullptr) {
        //positionList.reset(new std::vector<QPointF>);
        std::shared_ptr<ICommand> c(new magicWandCommand(*activeImage, pos, *positionList, ui->actionisPremultiplied->isChecked(), currentTolerance));
        commandManager.execute(c);
        qDebug("magicWand");

        pendingSaveModifications = true;
        ui->actionSave->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(false);
        updateStatusBar();
    }
}

void MainWindow::on_actionDrawShapes_triggered()
{
    qDebug("ActionDrawShapes");

    selectTool(DRAWSHAPES);

    drawShapesFlag = ui->actionDrawShapes->isChecked();

    if (drawShapesFlag) {
        qDebug("DrawShapes - True");
        pathItem->show();
        croppedImageItem->show();
    } else if (!drawShapesFlag) {
        qDebug("DrawShapes - False");
        resetLayers();
        resetButtons();
    }
}

void MainWindow::onDrawShapesOptionsToggled(bool checked)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action && action->text() == "Rectangle" && checked) {
        qDebug() << action->text() << "toggled on";
        ui->actionOptionCircle->setChecked(false);
        ui->actionOptionTriangle->setChecked(false);
    } else if (action && action->text() == "Triangle" && checked) {
        qDebug() << action->text() << "toggled on";
        ui->actionOptionCircle->setChecked(false);
        ui->actionOptionRectangle->setChecked(false);
    } else if (action && action->text() == "Circle" && checked) {
        qDebug() << action->text() << "toggled on";
        ui->actionOptionTriangle->setChecked(false);
        ui->actionOptionRectangle->setChecked(false);
    }
}

void MainWindow::on_actionSetTolerance_triggered()
{
    bool ok = false;
    QList<QString> field = {"Tolerance"};
    QList<int> input = SliderDialog::getFields(this, field, 1, MAX_TOLERANCE, 1, &ok);

    if (ok) {
        currentTolerance = input[0];
        qDebug("currentTolerance: %d", currentTolerance);
    }
}

void MainWindow::on_ToolSlider_valueChanged(int value)
{
    if (magicWandFlag) {
        ui->ToolLabel->setText((QString("%1").arg(value)));
        currentTolerance = value;
    } else if (brushFlag) {
        ui->ToolLabel->setText((QString("%1").arg(value)));
        brushSize = value;
    } else if (eraserFlag) {
        ui->ToolLabel->setText((QString("%1").arg(value)));
        eraserSize = value;
    }
}

void MainWindow::on_ToolButton_toggled(bool checked)
{
    if (magicWandFlag) {
        isMagicRemove = checked;
        ui->ToolButton->setText(isMagicRemove ? QString("Remove") : QString("Append"));
    }
}
