QT += widgets opengl

TEMPLATE = lib
DEFINES += PLUGINAUTOPAGE_LIBRARY

DESTDIR = $$PWD/../../bin/Plugin
MOC_DIR     = $$PWD/../../temp/PluginAutoPage/moc
RCC_DIR     = $$PWD/../../temp/PluginAutoPage/rcc
UI_DIR      = $$PWD/../../temp/PluginAutoPage/ui
OBJECTS_DIR = $$PWD/../../temp/PluginAutoPage/obj

CONFIG += c++11

CONFIG += force_debug_info    # 带调试信息
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Od   #禁用优化

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../Common/Custom/Custom.pri)

INCLUDEPATH += $$PWD/../Common/Custom

SOURCES += \
    ../Common/WidgetLog.cpp \
    Basic/CameraPreviewForm/CameraPreviewForm.cpp \
    Basic/ProcessLogForm/WidgetLogAll.cpp \
    Basic/PlotShowForm/AritightPlotItem.cpp \
    WidgetAritightPlotForm.cpp \
    WidgetBtnPanel.cpp \
    WidgetProductChange.cpp \
    PluginAutoPage.cpp \
    WidgetAutoPage.cpp \
    WidgetStatusBar.cpp \
    WidgetTray.cpp

HEADERS += \
    ../../interface/coreinterface.h \
    ../../interface/plugininterface.h \
    ../Common/WidgetLog.h \
    Basic/CameraPreviewForm/CameraPreviewForm.h \
    Basic/ProcessLogForm/WidgetLogAll.h \
    Basic/PlotShowForm/AritightPlotItem.h \
    WidgetAritightPlotForm.h \
    WidgetBtnPanel.h \
    WidgetProductChange.h \
    PluginAutoPage.h \
    WidgetAutoPage.h \
    WidgetStatusBar.h \
    WidgetTray.h

INCLUDEPATH += \
    Basic/DataTabelForm/ \
    Basic/CameraPreviewForm/ \
    Basic/ProcessLogForm/ \
    Basic/PlotShowForm/ \

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target

FORMS += \
    ../Common/WidgetLog.ui \
    Basic/CameraPreviewForm/CameraPreviewForm.ui \
    Basic/ProcessLogForm/WidgetLogAll.ui \
    Basic/PlotShowForm/AritightPlotItem.ui \
    WidgetAritightPlotForm.ui \
    WidgetBtnPanel.ui \
    WidgetProductChange.ui \
    WidgetAutoPage.ui \
    WidgetStatusBar.ui \
    WidgetTray.ui

RESOURCES += \
    resource/qss.qrc

include(Product/Product.pri)

#事件循环库
win32:CONFIG(release, debug|release): LIBS += -L$$(PATH_VIS)/VisAppTool/ -lVisAppTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(PATH_VIS)/VisAppTool/ -lVisAppToold

INCLUDEPATH += $$(PATH_VIS)/VisAppTool
DEPENDPATH += $$(PATH_VIS)/VisAppTool

#相机显示界面库
win32:CONFIG(release, debug|release): LIBS += -L$$(PATH_VIS)/VisCommon/ -lVisCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(PATH_VIS)/VisCommon/ -lVisCommond

INCLUDEPATH += $$(PATH_VIS)/VisCommon
DEPENDPATH += $$(PATH_VIS)/VisCommon

#运动控制库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../3rd/VisMotorTool/ -lVisMotorTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../3rd/VisMotorTool/ -lVisMotorToold

INCLUDEPATH += $$PWD/../../3rd/VisMotorTool
DEPENDPATH += $$PWD/../../3rd/VisMotorTool

INCLUDEPATH += $$PWD/../Common/CurveView
win32: LIBS += -L$$PWD/../../bin/ -lVISFramePluginModel
INCLUDEPATH += $$PWD/../../VISFramePluginModel
DEPENDPATH += $$PWD/../../VISFramePluginModel
win32: LIBS += -L$$PWD/../../bin/Plugin -lPluginParam
INCLUDEPATH += $$PWD/../PluginParam
DEPENDPATH += $$PWD/../PluginParam
