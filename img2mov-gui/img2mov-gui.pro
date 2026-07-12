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
packagesExist(opencv5) {
    PKGCONFIG += opencv5
    message("Building with OpenCV 5")
} else:packagesExist(opencv4) {
    PKGCONFIG += opencv4
    message("Building with OpenCV 4")
} else {
    error("OpenCV 4 or 5 development package not found")
}

