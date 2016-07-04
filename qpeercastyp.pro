VERSION = 0.5.0
DEBVERSION = 1
CONFIG -= debug
QT += network
TEMPLATE = app

DEPENDPATH += . src
INCLUDEPATH += . src

DEFINES += VERSION=\\\"$$VERSION\\\"

macx {
    TARGET = QPeerCastYP
    CONFIG += app_bundle
    BUILD_DIR = build-macx
    ICON = images/qpeercastyp.icns
}

unix:!macx {
    TARGET = qpeercastyp
    BUILD_DIR = build-unix
    LIBS += -lX11

    include(conf.pri)
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    isEmpty(BINDIR) {
        BINDIR = $$PREFIX/bin
    }
    isEmpty(DATADIR) {
        DATADIR = $$PREFIX/share
    }

    bin.files = $$TARGET
    bin.path = $$BINDIR

    desktop.files = src/qpeercastyp.desktop
    desktop.path = $$DATADIR/applications

    pixmap.files = images/qpeercastyp.png
    pixmap.path = $$DATADIR/pixmaps

    INSTALLS += bin desktop pixmap
}

unix {
    archive.target = archive
    archive.commands = git archive --format=tar --prefix=qpeercastyp-$${VERSION}/ HEAD \
                       | bzip2 > qpeercastyp-$${VERSION}.tar.bz2
    QMAKE_EXTRA_TARGETS += archive
}

linux-* {
    debchg.target = debchange
    debchg.commands = debchange -v $${VERSION}-$${DEBVERSION}
    QMAKE_EXTRA_TARGETS += debchg

    debsrc.target = deb-src
    debsrc.commands = tar jxvf qpeercastyp-$${VERSION}.tar.bz2 && \
                   cd qpeercastyp-$${VERSION}/ && \
                   debuild -S -sd
    QMAKE_EXTRA_TARGETS += debsrc

    deb.target = deb
    deb.commands = tar jxvf qpeercastyp-$${VERSION}.tar.bz2 && \
                   cd qpeercastyp-$${VERSION}/ && \
                   debuild -e MAKE=\"make -j3\" -b
    QMAKE_EXTRA_TARGETS += deb
}

win32 {
    TARGET = qpeercastyp
    BUILD_DIR = build-win32
    CONFIG += release
    CONFIG += static
    LIBS += -lgdi32
    # contains(CONFIG, debug):CONFIG += console
    RC_FILE = qpeercastyp_resource.rc

    nsis.target = nsis
    nsis.commands = \"C:\Program Files\NSIS\makensis.exe\" -DPRODUCT_VERSION=$${VERSION} qpeercastyp.nsi
    QMAKE_EXTRA_TARGETS += nsis
}

isEmpty(BUILD_DIR) {
    BUILD_DIR = build
}
MOC_DIR        = $$BUILD_DIR
OBJECTS_DIR    = $$BUILD_DIR
UI_DIR         = $$BUILD_DIR
UI_HEADERS_DIR = $$BUILD_DIR
UI_SOURCES_DIR = $$BUILD_DIR
RCC_DIR        = $$BUILD_DIR

CONFIG += precompile_header
PRECOMPILED_HEADER = stable.h

RESOURCES = qpeercastyp.qrc

CODECFORTR = UTF-8
TRANSLATIONS = qpeercastyp.ts

HEADERS += network.h \
           utils.h \
           sound.h \
           settingwidget.h \
           tooltip.h \
           viemacsbindings.h

SOURCES += network.cpp \
           utils.cpp \
           sound.cpp \
           settingwidget.cpp \
           tooltip.cpp \
           viemacsbindings.cpp

HEADERS += application.h \
           settings.h \
           settingsconverter.h \
           mainwindow.h \
           systemtrayicon.h \
           actions.h \
           commandaction.h \
           process.h \
           settingsdialog.h \
           generalwidget.h \
           yellowpageedit.h \
           yellowpagedialog.h \
           favoriteedit.h \
           favoritegroupdialog.h \
           expressiondialog.h \
           playeredit.h \
           notificationwidget.h \
           networkwidget.h \
           useractions.h \
           useractionedit.h \
           commandactiondialog.h \
           channel.h \
           channelmatcher.h \
           channellistwidget.h \
           channellisttabwidget.h \
           channellistfindbar.h \
           yellowpage.h \
           yellowpagemanager.h

SOURCES += main.cpp \
           application.cpp \
           settings.cpp \
           settingsconverter.cpp \
           mainwindow.cpp \
           systemtrayicon.cpp \
           actions.cpp \
           commandaction.cpp \
           process.cpp \
           settingsdialog.cpp \
           generalwidget.cpp \
           yellowpageedit.cpp \
           yellowpagedialog.cpp \
           favoriteedit.cpp \
           favoritegroupdialog.cpp \
           expressiondialog.cpp \
           playeredit.cpp \
           notificationwidget.cpp \
           useractions.cpp \
           useractionedit.cpp \
           commandactiondialog.cpp \
           networkwidget.cpp \
           channel.cpp \
           channelmatcher.cpp \
           channellistwidget.cpp \
           channellisttabwidget.cpp \
           channellistfindbar.cpp \
           yellowpage.cpp \
           yellowpagemanager.cpp

FORMS +=   channellistfindbar.ui \
           generalwidget.ui \
           yellowpageedit.ui \
           yellowpagedialog.ui \
           favoriteedit.ui \
           favoritegroupdialog.ui \
           expressiondialog.ui \
           playeredit.ui \
           notificationwidget.ui \
           networkwidget.ui \
           useractionedit.ui \
           commandactiondialog.ui \
           aboutqpeercastyp.ui

