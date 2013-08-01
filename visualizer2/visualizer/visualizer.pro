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
    stream.hpp \
    config.hpp \
    controller.hpp \
    main.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    window.cpp \
    camera.cpp \
    stream.cpp

LIBS += \
    -lGLU \
    -lssm
