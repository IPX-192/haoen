QT += core gui
QT += charts

HEADERS += \
    $$PWD/CRealTimeCurve.h \
    $$PWD/TrayCalculate.h
SOURCES += \
    $$PWD/CRealTimeCurve.cpp \
    $$PWD/TrayCalculate.cpp

FORMS += \
    $$PWD/CRealTimeCurve.ui

INCLUDEPATH += \
    $$PWD/

CONFIG += force_debug_info    # 带调试信息
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Od   #禁用优化
