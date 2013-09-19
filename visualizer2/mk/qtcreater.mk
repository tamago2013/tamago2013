#############################################################################
# Makefile for building: visualizer
# Generated by qmake (2.01a) (Qt 4.8.1) on: Thu Sep 19 14:35:15 2013
# Project:  ../visualizer/visualizer.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../visualizer/visualizer.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I../visualizer -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtOpenGL -I/usr/include/qt4 -I../../tkglib -I../../gndlib -I../../ssmtype -I/usr/X11R6/include -I. -I../visualizer -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -L/usr/X11R6/lib -lGLU -lssm -lQtOpenGL -lQtGui -lQtCore -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ../visualizer/main.cpp \
		../visualizer/window.cpp \
		../visualizer/camera.cpp \
		../visualizer/widget-gl.cpp \
		../visualizer/widget-msg.cpp \
		../visualizer/widget-img.cpp \
		../visualizer/ssm-message.cpp \
		../visualizer/menu-handler.cpp \
		../visualizer/map-viewer.cpp moc_camera.cpp \
		moc_window.cpp \
		moc_widget-msg.cpp \
		moc_widget-img.cpp \
		moc_widget-gl.cpp \
		moc_menu-handler.cpp \
		moc_map-viewer.cpp
OBJECTS       = main.o \
		window.o \
		camera.o \
		widget-gl.o \
		widget-msg.o \
		widget-img.o \
		ssm-message.o \
		menu-handler.o \
		map-viewer.o \
		moc_camera.o \
		moc_window.o \
		moc_widget-msg.o \
		moc_widget-img.o \
		moc_widget-gl.o \
		moc_menu-handler.o \
		moc_map-viewer.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		../visualizer/visualizer.pro
QMAKE_TARGET  = visualizer
DESTDIR       = 
TARGET        = visualizer

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: ../visualizer/visualizer.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtOpenGL.prl \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../visualizer/visualizer.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtOpenGL.prl:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile ../visualizer/visualizer.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/visualizer1.0.0 || $(MKDIR) .tmp/visualizer1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/visualizer1.0.0/ && $(COPY_FILE) --parents ../visualizer/camera.hpp ../visualizer/window.hpp ../visualizer/controller.hpp ../visualizer/main.hpp ../visualizer/widget-msg.hpp ../visualizer/widget-img.hpp ../visualizer/ssm-message.hpp ../visualizer/widget-gl.hpp ../../tkglib/tkg-utility.hpp ../../tkglib/tkg-opengl.hpp ../../tkglib/tkg-geometry.hpp ../../tkglib/tkg-config.hpp ../visualizer/menu-handler.hpp ../visualizer/map-viewer.hpp ../../tkglib/tkg-debug.hpp .tmp/visualizer1.0.0/ && $(COPY_FILE) --parents ../visualizer/main.cpp ../visualizer/window.cpp ../visualizer/camera.cpp ../visualizer/widget-gl.cpp ../visualizer/widget-msg.cpp ../visualizer/widget-img.cpp ../visualizer/ssm-message.cpp ../visualizer/menu-handler.cpp ../visualizer/map-viewer.cpp .tmp/visualizer1.0.0/ && (cd `dirname .tmp/visualizer1.0.0` && $(TAR) visualizer1.0.0.tar visualizer1.0.0 && $(COMPRESS) visualizer1.0.0.tar) && $(MOVE) `dirname .tmp/visualizer1.0.0`/visualizer1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/visualizer1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_camera.cpp moc_window.cpp moc_widget-msg.cpp moc_widget-img.cpp moc_widget-gl.cpp moc_menu-handler.cpp moc_map-viewer.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_camera.cpp moc_window.cpp moc_widget-msg.cpp moc_widget-img.cpp moc_widget-gl.cpp moc_menu-handler.cpp moc_map-viewer.cpp
moc_camera.cpp: ../visualizer/camera.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/camera.hpp -o moc_camera.cpp

moc_window.cpp: ../visualizer/window.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/window.hpp -o moc_window.cpp

moc_widget-msg.cpp: ../visualizer/widget-msg.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/widget-msg.hpp -o moc_widget-msg.cpp

moc_widget-img.cpp: ../visualizer/widget-img.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/widget-img.hpp -o moc_widget-img.cpp

moc_widget-gl.cpp: ../visualizer/camera.hpp \
		../visualizer/widget-gl.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/widget-gl.hpp -o moc_widget-gl.cpp

moc_menu-handler.cpp: ../visualizer/menu-handler.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/menu-handler.hpp -o moc_menu-handler.cpp

moc_map-viewer.cpp: ../visualizer/map-viewer.hpp
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) ../visualizer/map-viewer.hpp -o moc_map-viewer.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

main.o: ../visualizer/main.cpp ../visualizer/main.hpp \
		../visualizer/window.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o ../visualizer/main.cpp

window.o: ../visualizer/window.cpp ../visualizer/window.hpp \
		../visualizer/widget-gl.hpp \
		../visualizer/camera.hpp \
		../visualizer/widget-msg.hpp \
		../visualizer/widget-img.hpp \
		../visualizer/menu-handler.hpp \
		../visualizer/ssm-message.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o window.o ../visualizer/window.cpp

camera.o: ../visualizer/camera.cpp ../visualizer/camera.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o camera.o ../visualizer/camera.cpp

widget-gl.o: ../visualizer/widget-gl.cpp ../visualizer/window.hpp \
		../visualizer/widget-gl.hpp \
		../visualizer/camera.hpp \
		../visualizer/widget-msg.hpp \
		../visualizer/menu-handler.hpp \
		../visualizer/map-viewer.hpp \
		../visualizer/ssm-message.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o widget-gl.o ../visualizer/widget-gl.cpp

widget-msg.o: ../visualizer/widget-msg.cpp ../visualizer/widget-msg.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o widget-msg.o ../visualizer/widget-msg.cpp

widget-img.o: ../visualizer/widget-img.cpp ../visualizer/window.hpp \
		../visualizer/widget-img.hpp \
		../visualizer/menu-handler.hpp \
		../visualizer/ssm-message.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o widget-img.o ../visualizer/widget-img.cpp

ssm-message.o: ../visualizer/ssm-message.cpp ../visualizer/widget-msg.hpp \
		../visualizer/ssm-message.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ssm-message.o ../visualizer/ssm-message.cpp

menu-handler.o: ../visualizer/menu-handler.cpp ../visualizer/menu-handler.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o menu-handler.o ../visualizer/menu-handler.cpp

map-viewer.o: ../visualizer/map-viewer.cpp ../visualizer/map-viewer.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o map-viewer.o ../visualizer/map-viewer.cpp

moc_camera.o: moc_camera.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_camera.o moc_camera.cpp

moc_window.o: moc_window.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_window.o moc_window.cpp

moc_widget-msg.o: moc_widget-msg.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_widget-msg.o moc_widget-msg.cpp

moc_widget-img.o: moc_widget-img.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_widget-img.o moc_widget-img.cpp

moc_widget-gl.o: moc_widget-gl.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_widget-gl.o moc_widget-gl.cpp

moc_menu-handler.o: moc_menu-handler.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_menu-handler.o moc_menu-handler.cpp

moc_map-viewer.o: moc_map-viewer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_map-viewer.o moc_map-viewer.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:
