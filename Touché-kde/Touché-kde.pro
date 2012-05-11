#-------------------------------------------------
#
# Project created by QtCreator 2012-05-02T15:17:10
#
#-------------------------------------------------

QT       += core gui

TARGET = Touché
TEMPLATE = app


SOURCES += main.cpp \
    trayIcon/touchesystemtray.cpp

HEADERS  += \
    trayIcon/touchesystemtray.h \
    trayIcon/traymanager.h

HEADERS  +=


CONFIG(debug, debug|release)   { COMPILE_MODE='debug' }
   else                        { COMPILE_MODE='release' }


isEmpty(TMP_INSTALL_DIR) {
     TMP_INSTALL_DIR="/tmp"
 }

LIBS += -L$$TMP_INSTALL_DIR/Touché_Core/$$COMPILE_MODE -lTouché_Core -L$$TMP_INSTALL_DIR/Touché_Configuration/$$COMPILE_MODE -lTouché_Configuration
LIBS += -lqjson -lX11 -lXtst -lkdeui -lkdecore
QMAKE_CXXFLAGS += "-std=c++0x"

INCLUDEPATH += ../Touché_Core ../Touché_Configuration


target.path=/usr/bin
udev_rules.files=files/99-hiddev-input.rules
udev_rules.path=/etc/udev/rules.d

keyboard_database.files=files/keyboard_database.json
keyboard_database.path=/usr/share/$$TARGET/

desktop_entry.files += desktop/Touché.desktop
desktop_entry.path = /usr/share/applications/

INSTALLS += target udev_rules keyboard_database desktop_entry

