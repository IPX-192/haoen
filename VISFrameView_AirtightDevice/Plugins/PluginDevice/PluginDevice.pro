QT += widgets
QT += network
QT += serialport
QT += serialbus
TEMPLATE = lib
DEFINES += PLUGINDEVICE_LIBRARY

DESTDIR = $$PWD/../../bin/Plugin
MOC_DIR     = $$PWD/../../temp/PluginDevice/moc
RCC_DIR     = $$PWD/../../temp/PluginDevice/rcc
UI_DIR      = $$PWD/../../temp/PluginDevice/ui
OBJECTS_DIR = $$PWD/../../temp/PluginDevice/obj

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
    AirtightTest/CModbusClient.cpp \
    DeviceManage.cpp \
    PluginDevice.cpp \
    ScanCode/ScanCodeClient.cpp

HEADERS += \
    ../../interface/coreinterface.h \
    ../../interface/plugininterface.h \
    ../../interface/singleton.h \
    AirtightTest/CModbusClient.h \
    DeviceManage.h \
    LogDefine.h \
    PluginDevice.h \
    ScanCode/ScanCodeClient.h

INCLUDEPATH += \
    ../../interface

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

#事件循环库
win32:CONFIG(release, debug|release): LIBS += -L$$(PATH_VIS)/VisAppTool/ -lVisAppTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(PATH_VIS)/VisAppTool/ -lVisAppToold

INCLUDEPATH += $$(PATH_VIS)/VisAppTool
DEPENDPATH += $$(PATH_VIS)/VisAppTool

#Opencv动态库
win32:CONFIG(release, debug|release): LIBS += -L$$(PATH_VIS)/opencv/lib/ -lopencv_world411
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(PATH_VIS)/opencv/lib/ -lopencv_world411d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../3rd/VisCameraTool/ -lVisCameraTool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../3rd/VisCameraTool/ -lVisCameraToold

INCLUDEPATH += $$PWD/../../3rd/VisCameraTool
DEPENDPATH += $$PWD/../../3rd/VisCameraTool

INCLUDEPATH += $$(PATH_VIS)/opencv/include
DEPENDPATH += $$(PATH_VIS)/opencv/
win32: LIBS += -L$$PWD/../../bin/ -lVISFramePluginModel
INCLUDEPATH += $$PWD/../../VISFramePluginModel
DEPENDPATH += $$PWD/../../VISFramePluginModel

win32: LIBS += -L$$PWD/../../bin/Plugin -lPluginParam
INCLUDEPATH += $$PWD/../PluginParam
DEPENDPATH += $$PWD/../PluginParam
