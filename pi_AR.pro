#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T09:24:42
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pi_AR
TEMPLATE = app

#INCLUDEPATH += /usr/include/GL/
#LIBS+=-lglut -lGL -lGLU -lm


CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`

SOURCES += main.cpp\
        mainwindow.cpp\
    mygl.cpp \
    marker.cpp \
    decode.cpp \
#	 files/Load3DS.cpp \
#    files/LoadOBJ.cpp \
#    MeshModel.cpp
#   grabthread.cpp
    #grabthread.cpp \
    #moc_grabthread.cpp
    load3ds.cpp \
    loadOBJ/objloader.cpp \
    loadOBJ/tgaimage.cpp

HEADERS  += mainwindow.h\
    mygl.h \
    marker.h \
    decode.h \
#	files/Load3DS.h \
#    files/LoadOBJ.h \
#    MeshModel.h
    #grabthread.h
#   grabthread.h \
#	shareData.h
    load3ds.h \
    loadOBJ/objloader.h \
    loadOBJ/reference.h \
    loadOBJ/tgaimage.h

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib \
    /usr/local/lib/libopencv_core.so \
	/usr/local/lib/libopencv_features2d.so \
	/usr/local/lib/libopencv_flann.so \
	/usr/local/lib/libopencv_highgui.so \
	/usr/local/lib/libopencv_imgcodecs.so \
	/usr/local/lib/libopencv_imgproc.so \
	/usr/local/lib/libopencv_ml.so \
	/usr/local/lib/libopencv_objdetect.so \
	/usr/local/lib/libopencv_photo.so \
	/usr/local/lib/libopencv_shape.so \
	/usr/local/lib/libopencv_stitching.so \
	/usr/local/lib/libopencv_superres.so \
	/usr/local/lib/libopencv_videoio.so \
	/usr/local/lib/libopencv_video.so \
    /usr/local/lib/libopencv_videostab.so \
    /usr/local/lib/libopencv_calib3d.so \
    -l3ds \
    -lGLU


FORMS    += mainwindow.ui

#unix:!macx: LIBS += -L$$PWD/../../../../../home/atom/Qt5.9.1/5.9.1/gcc_64/plugins/imageformats/ -lqtga

#INCLUDEPATH += $$PWD/../../../../../home/atom/Qt5.9.1/5.9.1/gcc_64/plugins/imageformats
#DEPENDPATH += $$PWD/../../../../../home/atom/Qt5.9.1/5.9.1/gcc_64/plugins/imageformats
