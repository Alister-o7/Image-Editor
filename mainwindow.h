#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneWheelEvent>
#include <QLabel>
#include <QCloseEvent>
#include <QPainter>
#include <QColorDialog>
#include <QSlider>
#include "Image.h"
#include "CommandPattern/Commands.h"

//QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
//QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum Tools { SELECT, MOVE, PAINT_BUCKET, BRUSH, ERASER, LASSO, MAGICWAND, DRAWSHAPES };

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionZoom_Adapt_triggered();

    void on_actionZoomInc_triggered();

    void on_actionZoomDec_triggered();

    void on_actionRotate_Clockwise_triggered();

    void on_actionRotate_anticlockwise_triggered();

    void on_actionFlip_x_triggered();

    void on_actionFlip_y_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionGrayscale_triggered();

    void on_actionBlur_triggered();

    void on_actionSharpen_triggered();

    void on_actionEmboss_triggered();

    void on_actionEdge_detect_triggered();

    void on_actionExit_triggered();

    void on_actionSave_as_triggered();

    void on_actionAbout_Image_Editor_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionContrast_triggered();

    void on_actionBrightness_triggered();

    void on_actionColormask_triggered();

    void on_actionScale_triggered();

    void on_actionAutoCrop_triggered();

    void on_actionMove_triggered();

    void on_actionSelect_triggered();

    void on_actionMergeLayers_triggered();

    void on_actionDeleteRect_triggered();

    void on_actionSaturation_triggered();

    void on_actionPaintBucket_triggered();

    void on_actionColorPicker_triggered();

    void on_actionBrush_triggered();

    void on_actionPixmapToImage_triggered();

    void on_actionToolsBrushOptions_triggered();

    void on_actionEraser_triggered();

    void on_actionGaussianBlur_triggered();

    void on_actionToolsEraserOptions_triggered();

    void on_actionLasso_triggered();

    void on_actionMagicWand_triggered();

    void on_actionDrawShapes_triggered();

    void onDrawShapesOptionsToggled(bool checked);

    void on_actionSetTolerance_triggered();

    void on_ToolSlider_valueChanged(int value);

    void on_ToolButton_toggled(bool checked);

protected slots:
    void zoomUpdate(bool increment);
    void resetLayers();
    void resetButtons();
    void resetToolBox();
    void modifyPathLayer(bool append);
    void setPathLayer();
    void paintBucket(QPointF pos);
    void selectTool(Tools tool);
    void magicWand(QPointF pos);

private:
    Ui::MainWindow *ui;
    bool eventFilter(QObject *object, QEvent *event) override;
    QGraphicsPixmapItem* pixmapItem;
    QGraphicsPixmapItem* croppedImageItem;
    QGraphicsRectItem* rectangleRemoveItem;
    QGraphicsPathItem* pathItem;
    QGraphicsRectItem* darkOverlayItem;
    QGraphicsScene scene;

    QString APP_NAME, APP_VERSION;

    const char* IMAGE_FORMATS_LIST = "*.bmp *.cur *.gif *.ico *.jpeg *.jpg *.pbm *.pgm *.png *.ppm *.svg *.svgz *.xbm *.xpm\n"
                                     "*.bmp\n*.cur\n*.gif\n*.ico\n*.jpeg\n*.jpg\n"
                                     "*.pbm\n*.pgm\n*.png\n*.ppm\n*.svg\n*.svgz\n*.xbm\n*.xpm";

    std::unique_ptr<Image> activeImage;
    std::unique_ptr<Image> painterImage;
    std::unique_ptr<Image> mergeImage;
    std::unique_ptr<std::vector<QPointF>> positionList;

    CommandManager commandManager;

    bool pendingSaveModifications;
    bool selectFlag = false;
    bool moveFlag = false;
    bool isMovingFlag = false;
    bool paintBucketFlag = false;
    bool brushFlag = false;
    bool fromSelectFlag = false;
    bool eraserFlag = false;
    bool lassoFlag = false;
    bool toPaintBucketFlag = false;
    bool magicWandFlag = false;
    bool magicWandWasTriggered = false;
    bool drawShapesFlag = false;
    bool isMultipleLasso = false;
    bool isMagicRemove = false;
    QPointF lastMouseLocation;

    QList<double> zoomList;

    QLabel imageName;
    QLabel imageSize;
    QLabel mousePosition;

    QPixmap bufferPix;
    QPainter *qPainter;
    QPainterPath *qPath;
    QPainterPath tempQPath;
    QColorDialog colorDialog;
    QColor currentColor = QColor(255,255,255,255);
    QPointF currentMousePosition = QPointF(0, 0);

    int brushSize = 1;
    int eraserSize = 1;
    int currentTolerance = 1;

    const int MAX_TOLERANCE = 15;

protected:
    void closeEvent(QCloseEvent *event) override;
    void updateStatusBar();
};

#endif // MAINWINDOW_H
