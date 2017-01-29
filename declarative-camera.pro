TEMPLATE=app
TARGET=declarative-camera

QT += quick qml multimedia texttospeech widgets
INCLUDEPATH += tiny-dnn
INCLUDEPATH += .

CONFIG += c++11

SOURCES += qmlcamera.cpp \
    imageprocess.cpp
RESOURCES += declarative-camera.qrc

QMAKE_INFO_PLIST = Info.plist

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/declarative-camera
INSTALLS += target

HEADERS += \
    imageprocess.h

msvc {
    QMAKE_CXXFLAGS += /bigobj /openmp /arch:AVX
}

gcc {
    QMAKE_CXXFLAGS += -std=c++11 -fopenmp
    QMAKE_LFLAGS +=  -fopenmp
}
