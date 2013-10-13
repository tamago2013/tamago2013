QT += \
    widgets \
    opengl

INCLUDEPATH += \
    ../../tkglib \
    ../../gndlib \
    ../../visualizer/src


HEADERS += \
    window.hpp \
    ../../tkglib/tkg-utility.hpp \
    ../../tkglib/tkg-opengl.hpp \
    ../../tkglib/tkg-geometry.hpp \
    ../../tkglib/tkg-debug.hpp \
    ../../tkglib/tkg-config.hpp \
    widget-msg.hpp \
    widget-gl.hpp \
    ../../visualizer/src/menu-handler.hpp \
    ../../visualizer/src/map-viewer.hpp \
    route-edit.hpp

SOURCES += \
    main.cpp \
    window.cpp \
    widget-msg.cpp \
    widget-gl.cpp \
    ../../visualizer/src/menu-handler.cpp \
    ../../visualizer/src/map-viewer.cpp \
    route-edit.cpp

LIBS += \
    -lGLU
