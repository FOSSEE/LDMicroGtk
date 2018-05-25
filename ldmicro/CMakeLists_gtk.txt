CMAKE_MINIMUM_REQUIRED (VERSION 3.1)

PROJECT (GTK3_TEST)

IF (NOT UNIX)
    set (PKG_CONFIG_PATH "C:/path/to/gtk/lib/pkgconfig")
    set (PATH "C:/path/to/gtk/bin;%PATH%")
ENDIF (NOT UNIX)

FIND_PACKAGE (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)

# Version control
SET (GTK3_TEST_VERSION_MAJOR 1)
SET (GTK3_TEST_VERSION_MINOR 0)

# add the binary tree to the search path for include files
SET (PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")
include_directories (${GTK3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES ("${CMAKE_CURRENT_SOURCE_DIR}/includes")


# configure a header file to pass some of the CMake settings
# to the source code
CONFIGURE_FILE (
"${PROJECT_INCLUDE_DIR}/gtk3.h.in"
"${PROJECT_INCLUDE_DIR}/gtk3.h"
)

LINK_DIRECTORIES (${GTK3_LIBRARY_DIRS})
ADD_EXECUTABLE(GTK3_TEST test.cpp)
ADD_DEFINITIONS (${GTK3_CFLAGS_OTHER})
TARGET_LINK_LIBRARIES (GTK3_TEST ${GTK3_LIBRARIES})
