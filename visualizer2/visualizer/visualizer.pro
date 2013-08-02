QT += \
    widgets \
    opengl

INCLUDEPATH += \
    ../../tkglib \
    ../../gndlib \
    ../../ssmtype

HEADERS += \
    glwidget.hpp \
    camera.hpp \
    window.hpp \
    config.hpp \
    controller.hpp \
    main.hpp \
    msgwidget.hpp \
    mapviewer.hpp \
    ssmviewer.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    window.cpp \
    camera.cpp \
    mapviewer.cpp \
    ssmviewer.cpp

LIBS += \
    -lGLU \
    -lssm
