QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS += imageEditor.ico

SOURCES += \
    CommandPattern/CommandManager.cpp \
    CommandPattern/autoCropCommand.cpp \
    CommandPattern/blurCommand.cpp \
    CommandPattern/blurGaussianCommand.cpp \
    CommandPattern/brightnessCommand.cpp \
    CommandPattern/colorMaskCommand.cpp \
    CommandPattern/colorSaturationCommand.cpp \
    CommandPattern/contrastCommand.cpp \
    CommandPattern/deleteRectCommand.cpp \
    CommandPattern/edgeDetectCommand.cpp \
    CommandPattern/embossCommand.cpp \
    CommandPattern/flipXCommand.cpp \
    CommandPattern/flipYCommand.cpp \
    CommandPattern/grayScaleCommand.cpp \
    CommandPattern/magicWandCommand.cpp \
    CommandPattern/mergeLayersCommand.cpp \
    CommandPattern/paintBucketCommand.cpp \
    CommandPattern/pixmapToImageCommand.cpp \
    CommandPattern/rotateAntiClockwiseCommand.cpp \
    CommandPattern/rotateClockwiseCommand.cpp \
    CommandPattern/scaleCommand.cpp \
    CommandPattern/sharpenCommand.cpp \
    Image.cpp \
    InputDialog.cpp \
    Pixel.cpp \
    SliderDialog.cpp \
    main.cpp \
    mainwindow.cpp \
    utils.cpp

HEADERS += \
    CommandPattern/CommandManager.h \
    CommandPattern/Commands.h \
    CommandPattern/ICommand.h \
    CommandPattern/autoCropCommand.h \
    CommandPattern/blurCommand.h \
    CommandPattern/blurGaussianCommand.h \
    CommandPattern/brightnessCommand.h \
    CommandPattern/colorMaskCommand.h \
    CommandPattern/colorSaturationCommand.h \
    CommandPattern/contrastCommand.h \
    CommandPattern/deleteRectCommand.h \
    CommandPattern/edgeDetectCommand.h \
    CommandPattern/embossCommand.h \
    CommandPattern/flipXCommand.h \
    CommandPattern/flipYCommand.h \
    CommandPattern/grayScaleCommand.h \
    CommandPattern/magicWandCommand.h \
    CommandPattern/mergeLayersCommand.h \
    CommandPattern/paintBucketCommand.h \
    CommandPattern/pixmapToImageCommand.h \
    CommandPattern/rotateAntiClockwiseCommand.h \
    CommandPattern/rotateClockwiseCommand.h \
    CommandPattern/scaleCommand.h \
    CommandPattern/sharpenCommand.h \
    Commands.h \
    Image.h \
    InputDialog.h \
    Pixel.h \
    SliderDialog.h \
    mainwindow.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/credits/icons_license.pdf \
    resources/credits/license.html \
    resources/icon.png \
    resources/redo.png \
    resources/undo.png \
    resources/zoom-adapt.png \
    resources/zoom-in.png \
    resources/zoom-out.png

RESOURCES += \
    resources.qrc
