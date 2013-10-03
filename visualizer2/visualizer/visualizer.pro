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
    widget-gl.hpp \
    ../../tkglib/tkg-utility.hpp \
    ../../tkglib/tkg-opengl.hpp \
    ../../tkglib/tkg-geometry.hpp \
    ../../tkglib/tkg-config.hpp \
    menu-handler.hpp \
    map-viewer.hpp \
    ../../tkglib/tkg-debug.hpp \
    structure.hpp

SOURCES += \
    main.cpp \
    window.cpp \
    camera.cpp \
    widget-gl.cpp \
    widget-msg.cpp \
    widget-img.cpp \
    ssm-message.cpp \
    menu-handler.cpp \
    map-viewer.cpp

LIBS += \
    -lGLU \
    -lssm

OTHER_FILES += \
    memo.txt
