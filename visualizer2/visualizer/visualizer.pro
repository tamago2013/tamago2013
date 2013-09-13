QT += \
    widgets \
    opengl

INCLUDEPATH += \
    ../../tkglib \
    ../../gndlib \
    ../../ssmtype

HEADERS += \
    camera.hpp \
    window.hpp \
    controller.hpp \
    main.hpp \
    widget-msg.hpp \
    widget-img.hpp \
    ssm-message.hpp \
    fps-timer.hpp \
    widget-gl.hpp \
    ../../tkglib/tkg-utility.hpp \
    ../../tkglib/tkg-opengl.hpp \
    ../../tkglib/tkg-geometry.hpp \
    ../../tkglib/tkg-config.hpp

SOURCES += \
    main.cpp \
    window.cpp \
    camera.cpp \
    widget-gl.cpp \
    widget-msg.cpp \
    widget-img.cpp \
    ssm-message.cpp \
    fps-timer.cpp

LIBS += \
    -lGLU \
    -lssm
