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
    widget-gl.hpp \
    widget-msg.hpp \
    viewer-map.hpp \
    widget-img.hpp \
    ssm-message.hpp \
    tkg-config.hpp \
    tkg-utility.hpp \
    fps-timer.hpp

SOURCES += \
    main.cpp \
    window.cpp \
    camera.cpp \
    widget-gl.cpp \
    viewer-map.cpp \
    widget-msg.cpp \
    widget-img.cpp \
    ssm-message.cpp \
    fps-timer.cpp

LIBS += \
    -lGLU \
    -lssm
