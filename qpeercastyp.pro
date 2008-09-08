VERSION = 0.1.0
DEFINES += VERSION=\\\"$$VERSION\\\"
QT += network
TEMPLATE = app
TARGET = qpeercastyp
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += precompile_header
PRECOMPILED_HEADER = stable.h

unix {
    CONFIG += debug
    # CONFIG -= debug
    # CONFIG += release
    BUILD_DIR = build-linux

    # gprof 用
    # QMAKE_CXXFLAGS_DEBUG += -pg
    # QMAKE_LFLAGS_DEBUG += -pg

    contains(CONFIG, debug) {
        run.target = all
        run.commands = ./$$TARGET
        QMAKE_EXTRA_TARGETS += run
    }

    archive.target = archive
    archive.commands = git-archive --format=tar --prefix=qpeercastyp-$${VERSION}/ HEAD \
                       | bzip2 > qpeercastyp-$${VERSION}.tar.bz2

    QMAKE_EXTRA_TARGETS += archive

    HEADERS += proxystyle.h
    SOURCES += proxystyle.cpp

    include(conf.pri)

    bin.files = $$TARGET
    bin.path = $$BINDIR

    pcraw.files = pcraw/pcraw_srv pcraw/pcraw_proxy
    pcraw.path = $$BINDIR

    INSTALLS += bin pcraw
}

win32 {
    CONFIG += static release
    # CONFIG += console
    BUILD_DIR = build-win32
    RC_FILE = qpeercastyp_resource.rc

    nsis.target = nsis
    nsis.commands = \"C:\Program Files\NSIS\makensis.exe\" -DPRODUCT_VERSION=$${VERSION} qpeercastyp.nsi
    QMAKE_EXTRA_TARGETS += nsis
}

MOC_DIR        = $$BUILD_DIR
OBJECTS_DIR    = $$BUILD_DIR
UI_DIR         = $$BUILD_DIR
UI_HEADERS_DIR = $$BUILD_DIR
UI_SOURCES_DIR = $$BUILD_DIR
RCC_DIR        = $$BUILD_DIR

HEADERS += network.h \
           utils.h \
           sound.h \
           settingwidget.h \
           viemacsbindings.h

SOURCES += network.cpp \
           utils.cpp \
           sound.cpp \
           settingwidget.cpp \
           viemacsbindings.cpp

HEADERS += application.h \
           settings.h \
           mainwindow.h \
           systemtrayicon.h \
           actions.h \
           settingsdialog.h \
           generalwidget.h \
           yellowpageedit.h \
           yellowpagedialog.h \
           favoriteedit.h \
           expressiondialog.h \
           playeredit.h \
           notificationwidget.h \
           networkwidget.h \
           advancedwidget.h \
           channel.h \
           channelmatcher.h \
           channellistwidget.h \
           channellisttabwidget.h \ 
           channellistfindbar.h \
           yellowpage.h \
           yellowpagemanager.h \
           pcrawproxy.h

SOURCES += main.cpp \
           application.cpp \
           settings.cpp \
           mainwindow.cpp \
           systemtrayicon.cpp \
           actions.cpp \
           settingsdialog.cpp \
           generalwidget.cpp \
           yellowpageedit.cpp \
           yellowpagedialog.cpp \
           favoriteedit.cpp \
           expressiondialog.cpp \
           playeredit.cpp \
           notificationwidget.cpp \
           networkwidget.cpp \
           advancedwidget.cpp \
           channel.cpp \
           channelmatcher.cpp \
           channellistwidget.cpp \
           channellisttabwidget.cpp \ 
           channellistfindbar.cpp \
           yellowpage.cpp \
           yellowpagemanager.cpp \
           pcrawproxy.cpp

FORMS += channellistfindbar.ui \
         generalwidget.ui \
         yellowpageedit.ui \
         yellowpagedialog.ui \
         favoriteedit.ui \
         expressiondialog.ui \
         playeredit.ui \
         notificationwidget.ui \
         networkwidget.ui \
         advancedwidget.ui \
         aboutqpeercastyp.ui

RESOURCES = qpeercastyp.qrc

