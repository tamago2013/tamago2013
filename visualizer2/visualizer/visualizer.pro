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
    window.hpp

SOURCES += \
    main.cpp \
    glwidget.cpp \
    window.cpp \
    camera.cpp

LIBS += \
    -lGLU \
    -lssm
