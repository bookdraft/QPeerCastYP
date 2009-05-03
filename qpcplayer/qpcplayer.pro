TEMPLATE = app
TARGET = qpcplayer
DEPENDPATH += .
INCLUDEPATH += .

# run.target = all
# run.commands = ./$$TARGET \"/dev/video0\" -window-title TV
# contains(CONFIG, debug):QMAKE_EXTRA_TARGETS += run

SOURCES += main.cpp qpcplayer.cpp
HEADERS += qpcplayer.h

!include(conf.pri) {
    PREFIX = /usr/local
    BINDIR = /usr/local/bin
    DATADIR = /usr/local/share
}

bin.files += $$TARGET
bin.path = $$BINDIR
INSTALLS += bin

