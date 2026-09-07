QT += core gui

HEADERS += \
    $$PWD/CLoadingWidget.h \
    $$PWD/CMsgBox.h \
    $$PWD/CMulComboBox.h \
    $$PWD/CMulComboBoxSearch.h \
    $$PWD/CMulComboBoxSearchModel.h \
    $$PWD/CMulNumLineEdit.h \
    $$PWD/CMulTextLineEdit.h \
    $$PWD/CPopTipScreen.h \
    $$PWD/CTimeSelectWidget.h \
    $$PWD/CutomDoubleSinBox.h \
    $$PWD/CutomSinBox.h \
    $$PWD/NonBlockingMsgBox.h \
    $$PWD/WearingParts.h \
    $$PWD/WearingPartsList.h \
    $$PWD/WearingPartsListUser.h
#    $$PWD/TrayCtrl.h \
SOURCES += \
    $$PWD/CLoadingWidget.cpp \
    $$PWD/CMsgBox.cpp \
    $$PWD/CMulComboBox.cpp \
    $$PWD/CMulComboBoxSearch.cpp \
    $$PWD/CMulComboBoxSearchModel.cpp \
    $$PWD/CMulNumLineEdit.cpp \
    $$PWD/CMulTextLineEdit.cpp \
    $$PWD/CPopTipScreen.cpp \
    $$PWD/CTimeSelectWidget.cpp \
    $$PWD/CutomDoubleSinBox.cpp \
    $$PWD/CutomSinBox.cpp \
    $$PWD/NonBlockingMsgBox.cpp \
    $$PWD/WearingParts.cpp \
    $$PWD/WearingPartsList.cpp \
    $$PWD/WearingPartsListUser.cpp
#    $$PWD/TrayCtrl.cpp \

FORMS += \
    $$PWD/WearingParts.ui \
    $$PWD/WearingPartsList.ui \
    $$PWD/WearingPartsListUser.ui
#    $$PWD/TrayCtrl.ui \

RESOURCES += \
    $$PWD/CustomRec.qrc
