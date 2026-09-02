QT += widgets serialport network

TEMPLATE = lib
DEFINES += PLUGINSYSTEMSET_LIBRARY

DESTDIR = $$PWD/../../bin/Plugin
MOC_DIR     = $$PWD/../../temp/PluginSystemSet/moc
RCC_DIR     = $$PWD/../../temp/PluginSystemSet/rcc
UI_DIR      = $$PWD/../../temp/PluginSystemSet/ui
OBJECTS_DIR = $$PWD/../../temp/PluginSystemSet/obj

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

SOURCES += \
    ../Common/WidgetLog.cpp \
    DataSaveForm.cpp \
    Item/ElectricGripperParamForm.cpp \
    MES/FreetechMesManage.cpp \
    MES/MesManager.cpp \
    MesParamForm.cpp \
    Item/SerialComForm.cpp \
    PluginSystemSet.cpp \
    Shield/buttondelegate.cpp \
    Shield/shieldtablemodel.cpp \
    WidgetAritightForm.cpp \
    WidgetAritightForm.cpp \
    WidgetGripper.cpp \
    WidgetHardWareDelay.cpp \
    WidgetShield.cpp \
    WidgetSystemSet.cpp

HEADERS += \
    ../../interface/coreinterface.h \
    ../../interface/plugininterface.h \
    ../Common/WidgetLog.h \
    DataSaveForm.h \
    Item/ElectricGripperParamForm.h \
    MES/FreetechMesManage.h \
    MES/MesManager.h \
    MES/messageStruct.h \
    MesParamForm.h \
    Item/SerialComForm.h \
    PluginSystemSet.h \
    Shield/buttondelegate.h \
    Shield/shieldtablemodel.h \
    WidgetAritightForm.h \
    WidgetAritightForm.h \
    WidgetGripper.h \
    WidgetHardWareDelay.h \
    WidgetShield.h \
    WidgetSystemSet.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ../Common/WidgetLog.ui \
    DataSaveForm.ui \
    Item/ElectricGripperParamForm.ui \
    MesParamForm.ui \
    Item/SerialComForm.ui \
    WidgetAritightForm.ui \
    WidgetAritightForm.ui \
    WidgetGripper.ui \
    WidgetHardWareDelay.ui \
    WidgetSystemSet.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rd/VisAppTool/ -lVisAppTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rd/VisAppTool/ -lVisAppToold

INCLUDEPATH += $$PWD/../../../3rd/VisAppTool
DEPENDPATH += $$PWD/../../../3rd/VisAppTool

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
