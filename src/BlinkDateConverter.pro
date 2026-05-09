QT       += core gui widgets

CONFIG += c++17
CONFIG += warn_on

TARGET = BlinkDateConverter
TEMPLATE = app

# Build output folders (relative to project root)
DESTDIR = release
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    conversion.cpp

HEADERS += \
    mainwindow.h \
    conversion.h

# Windows: set icon
RC_ICONS = assets/app.ico

# MinGW static build hints (uncomment if you have static Qt)
# CONFIG += static
# QMAKE_LFLAGS += -static
