QT += widgets network serialport xml
QT += printsupport
TEMPLATE = lib
DEFINES += PLUGINPRODUCTSET_LIBRARY

DESTDIR = $$PWD/../../bin/Plugin
MOC_DIR     = $$PWD/../../temp/PluginProductSet/moc
RCC_DIR     = $$PWD/../../temp/PluginProductSet/rcc
UI_DIR      = $$PWD/../../temp/PluginProductSet/ui
OBJECTS_DIR = $$PWD/../../temp/PluginProductSet/obj

CONFIG += c++11

CONFIG += force_debug_info    # 带调试信息
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Od   #禁用优化

include(../Common/Custom/Custom.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Common/WidgetLog.cpp \
    Item/AritightForm.cpp \
    ItemDelegate.cpp \
    PluginProductSet.cpp \
    WidgetAritight.cpp \
    WidgetMatrixCfg.cpp \
    WidgetProductSet.cpp \
    WidgetRecipeGrip.cpp \
    WidgetRecipeMotor.cpp \
    WidgetRecipePlatform.cpp \
    WidgetRecipeTray.cpp

HEADERS += \
    ../../interface/coreinterface.h \
    ../../interface/plugininterface.h \
    ../../interface/singleton.h \
    ../Common/FeildDefine.h \
    ../Common/WidgetLog.h \
    Item/AritightForm.h \
    ItemDelegate.h \
    PluginProductSet.h \
    WidgetAritight.h \
    WidgetMatrixCfg.h \
    WidgetProductSet.h \
    WidgetRecipeGrip.h \
    WidgetRecipeMotor.h \
    WidgetRecipePlatform.h \
    WidgetRecipeTray.h

INCLUDEPATH += \
    ../../interface/ \
    ../Common/ \

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ../Common/WidgetLog.ui \
    Item/AritightForm.ui \
    WidgetAritight.ui \
    WidgetMatrixCfg.ui \
    WidgetProductSet.ui \
    WidgetRecipeGrip.ui \
    WidgetRecipeMotor.ui \
    WidgetRecipePlatform.ui \
    WidgetRecipeTray.ui

#运动控制卡调机库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisPLCMotorTool/ -lVisPLCMotorTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisPLCMotorTool/ -lVisPLCMotorToold

INCLUDEPATH += $$PWD/../../../3rd/VisPLCMotorTool
DEPENDPATH += $$PWD/../../../3rd/VisPLCMotorTool

#事件循环库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisAppTool/ -lVisAppTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisAppTool/ -lVisAppToold

INCLUDEPATH += $$PWD/../../../3rd/VisAppTool
DEPENDPATH += $$PWD/../../../3rd/VisAppTool

#Opencv动态库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/opencv/lib/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/opencv/lib/ -lopencv_world411d

INCLUDEPATH += $$PWD/../../../3rd/opencv/include
DEPENDPATH += $$PWD/../../../3rd/opencv/lib

#相机库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisCameraTool/ -lVisCameraTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisCameraTool/ -lVisCameraToold

INCLUDEPATH += $$PWD/../../../3rd/VisCameraTool
DEPENDPATH += $$PWD/../../../3rd/VisCameraTool

#相机显示界面库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisCommon/ -lVisCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisCommon/ -lVisCommond

INCLUDEPATH += $$PWD/../../../3rd/VisCommon
DEPENDPATH += $$PWD/../../../3rd/VisCommon

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisCustomLib/ -lVisCustomPlugin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisCustomLib/ -lVisCustomPlugind

INCLUDEPATH += $$PWD/../../../3rd/VisCustomLib
DEPENDPATH += $$PWD/../../../3rd/VisCustomLib

win32: LIBS += -L$$PWD/../../bin/ -lVISFramePluginModel
INCLUDEPATH += $$PWD/../../VISFramePluginModel
DEPENDPATH += $$PWD/../../VISFramePluginModel
win32: LIBS += -L$$PWD/../../bin/Plugin -lPluginParam
INCLUDEPATH += $$PWD/../PluginParam
DEPENDPATH += $$PWD/../PluginParam
