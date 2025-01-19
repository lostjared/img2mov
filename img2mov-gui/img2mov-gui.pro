QT += core gui widgets

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    worker.cpp \
    ffmpeg_write.cpp

HEADERS += \
    mainwindow.h \
    worker.h \
    ffmpeg_write.h

CONFIG += c++17

CONFIG += link_pkgconfig
PKGCONFIG += opencv4

#INCLUDEPATH += /usr/include/opencv4
win32 {
#LIBS += ./libopencv_core.dll.a ./libopencv_imgcodecs.dll.a 
}
#LIBS += -lopencv_core -lopencv_imgcodecs

