VERSION = 0.4.2
DEBVERSION = 1
CONFIG -= debug
QT += network
TEMPLATE = app

DEPENDPATH += .
INCLUDEPATH += .

DEFINES += VERSION=\\\"$$VERSION\\\"

macx {
    TARGET = QPeerCastYP
    CONFIG += app_bundle
    BUILD_DIR = build-macx
    ICON = images/qpeercastyp.icns

    run.target = all
    run.commands = ./$${TARGET}.app/Contents/MacOS/QPeerCastYP
    contains(CONFIG, debug):QMAKE_EXTRA_TARGETS += run
}

linux-* {
    TARGET = qpeercastyp
    BUILD_DIR = build-linux

    PBUILDER_DISTS = intrepid hardy gutsy feisty
    PBUILDER_DIR = /var/cache/pbuilder/

    GOOGLECODE_UPLOAD = googlecode_upload.py -p qpeercastyp -u ciao.altern8

    !include(conf.pri) {
        PREFIX = /usr/local
        BINDIR = /usr/local/bin
        DATADIR = /usr/local/share
    }

    archive.target = archive
    archive.commands = git archive --format=tar --prefix=qpeercastyp-$${VERSION}/ HEAD \
                       | bzip2 > qpeercastyp-$${VERSION}.tar.bz2
    QMAKE_EXTRA_TARGETS += archive

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

    # pbuilder
    for(dist, PBUILDER_DISTS) {
        # create
        eval(pc$${dist}.target = pbuilder-create-$${dist})
        eval(pc$${dist}.commands = pbuilder create --distribution $$dist \
            --othermirror \\\"deb http://archive.ubuntu.com/ubuntu $$dist universe multiverse\\\" \
            --basetgz \\\"$${PBUILDER_DIR}/base-$${dist}.tgz\\\")
        eval(QMAKE_EXTRA_TARGETS += pc$${dist})

        # update
        eval(pu$${dist}.target = pbuilder-update-$${dist})
        eval(pu$${dist}.commands = pbuilder update \
            --basetgz \\\"$${PBUILDER_DIR}/base-$${dist}.tgz\\\")
        eval(QMAKE_EXTRA_TARGETS += pu$${dist})

        # build
        eval(pd$${dist}.target = pbuilder-build-$${dist})
        eval(pd$${dist}.commands = MAKE=\\\"make -j3\\\" pbuilder build \
            --basetgz \\\"$${PBUILDER_DIR}/base-$${dist}.tgz\\\" \
            --buildresult $${PBUILDER_DIR}/result-$${dist} \
            qpeercastyp_$${VERSION}-$${DEBVERSION}.dsc)
        eval(QMAKE_EXTRA_TARGETS += pd$${dist})
    }

    # http://code.google.com/p/support/wiki/ScriptedUploads
    uploadsrc.target = upload-src
    uploadsrc.commands = $$GOOGLECODE_UPLOAD -s \"QPeerCastYP $$VERSION - Source\" \
                         -l Type-Source,Featured qpeercastyp-$${VERSION}.tar.bz2
    uploaddeb.target = upload-deb
    uploaddeb.commands = $$GOOGLECODE_UPLOAD -s \"QPeerCastYP $$VERSION - Package for Ubuntu\" \
                         -l OpSys-Linux,Type-Package,Featured qpeercastyp_$${VERSION}-$${DEBVERSION}_i386.deb
    uploadexe.target = upload-installer
    uploadexe.commands = $$GOOGLECODE_UPLOAD -s \"QPeerCastYP $$VERSION - Installer for Windows\" \
                         -l OpSys-Windows,Type-Installer,Featured qpeercastyp-$${VERSION}.exe
    uploadall.target = upload-all
    uploadall.depends = uploadsrc uploaddeb uploadexe
    QMAKE_EXTRA_TARGETS += uploadsrc uploaddeb uploadexe uploadall

    run.target = all
    run.commands = ./$$TARGET
    contains(CONFIG, debug):QMAKE_EXTRA_TARGETS += run

    pcraw.target = pcraw-bin
    pcraw.commands = @cd pcraw; make
    QMAKE_EXTRA_TARGETS += pcraw

    bin.files = $$TARGET
    bin.path = $$BINDIR

    desktop.files = qpeercastyp.desktop
    desktop.path = $$DATADIR/applications

    pixmap.files = images/qpeercastyp.png
    pixmap.path = $$DATADIR/pixmaps

    INSTALLS += bin desktop pixmap

    exists(pcraw/pcraw_proxy) {
        pcraw.files = pcraw/pcraw_proxy
        pcraw.path = $$BINDIR
        INSTALLS += pcraw
    }
}

win32 {
    TARGET = qpeercastyp
    BUILD_DIR = build-win32
    CONFIG += release
    CONFIG += static
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

unix {
    HEADERS += proxystyle.h
    SOURCES += proxystyle.cpp
    LIBS += -lX11
}

win32 {
    HEADERS += explorerstyle.h
    SOURCES += explorerstyle.cpp
    LIBS += -lgdi32
}

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
           favoritegroupdialog.ui \
           expressiondialog.ui \
           playeredit.ui \
           notificationwidget.ui \
           networkwidget.ui \
           useractionedit.ui \
           commandactiondialog.ui \
           advancedwidget.ui \
           aboutqpeercastyp.ui

