TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio
