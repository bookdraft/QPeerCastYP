VERSION = 0.3.0
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
    BUILD_DIR = build-linux

    !include(conf.pri) {
        PREFIX = /usr/local
        BINDIR = /usr/local/bin
        DATADIR = /usr/local/share
    }

    # gprof 用
    # QMAKE_CXXFLAGS_DEBUG += -pg
    # QMAKE_LFLAGS_DEBUG += -pg

    run.target = all
    run.commands = ./$$TARGET
    QMAKE_EXTRA_TARGETS += run

    pcraw.target = pcraw-bin
    pcraw.commands = @cd pcraw; make
    QMAKE_EXTRA_TARGETS += pcraw

    archive.target = archive
    archive.commands = git-archive --format=tar --prefix=qpeercastyp-$${VERSION}/ HEAD \
                       | bzip2 > qpeercastyp-$${VERSION}.tar.bz2
    QMAKE_EXTRA_TARGETS += archive

    deb.target = deb
    deb.commands = tar jxvf qpeercastyp-$${VERSION}.tar.bz2 && \
                   cd qpeercastyp-$${VERSION}/ && \
                   debuild -e MAKE=\"make -j3\" -b -uc -us 
    QMAKE_EXTRA_TARGETS += deb

    # http://code.google.com/p/support/wiki/ScriptedUploads
    googlecode_upload = googlecode_upload.py -p qpeercastyp -u ciao.altern8
    uploadsrc.target = upload-src
    uploadsrc.commands = $$googlecode_upload -s \"QPeerCastYP $$VERSION - Source\" \
                         -l Type-Source,Featured qpeercastyp-$${VERSION}.tar.bz2
    uploaddeb.target = upload-deb
    uploaddeb.commands = $$googlecode_upload -s \"QPeerCastYP $$VERSION - Debian Package\" \
                         -l OpSys-Linux,Type-Package,Featured qpeercastyp_$${VERSION}-*_i386.deb
    uploadexe.target = upload-installer
    uploadexe.commands = $$googlecode_upload -s \"QPeerCastYP $$VERSION - Windows Installer\" \
                         -l OpSys-Windows,Type-Installer,Featured qpeercastyp-$${VERSION}.exe
    uploadall.target = upload-all
    uploadall.depends = uploadsrc uploaddeb uploadexe
    QMAKE_EXTRA_TARGETS += uploadsrc uploaddeb uploadexe uploadall

    HEADERS += proxystyle.h
    SOURCES += proxystyle.cpp

    bin.files = $$TARGET
    bin.path = $$BINDIR

    desktop.files = qpeercastyp.desktop
    desktop.path = $$DATADIR/applications

    pixmap.files = images/qpeercastyp.png
    pixmap.path = $$DATADIR/pixmaps

    INSTALLS += bin desktop pixmap

    exists (pcraw/pcraw_proxy) {
        pcraw.files = pcraw/pcraw_proxy
        pcraw.path = $$BINDIR
        INSTALLS += pcraw
    }
}

win32 {
    CONFIG += static release
    CONFIG += console
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
           settingsconverter.h \
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
           settingsconverter.cpp \
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

FORMS +=   channellistfindbar.ui \
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

