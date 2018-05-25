cmake_minimum_required (VERSION 2.6)

project (LDMicro)

IF (NOT UNIX)
    add_definitions (__WINDOWS_SYSTEM__)
    set (PKG_CONFIG_PATH "C:/path/to/gtk/lib/pkgconfig")
    set (PATH "C:/path/to/gtk/bin;%PATH%")
ENDIF (NOT UNIX)

MESSAGE ( STATUS "Searching for GTK+3.0..." )
find_package() (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)

# Version control
set (LDMicro_VERSION_MAJOR 1)
set (LDMicro_VERSION_MINOR 0)

# Add GTK3 include files if GTK3 is found

IF ( GTK3_FOUND )
      include_directories (${GTK3_INCLUDE_DIRS})
      link_directories (${GTK3_LIBRARY_DIRS})
      add_definitions (${GTK3_CFLAGS_OTHER})
      link_libraries (${GTK3_LIBRARIES})
      MESSAGE ( STATUS " GTK3_INCLUDE_DIR: " ${GTK3_INCLUDE_DIRS} )
      MESSAGE ( STATUS " GTK3_LIBRARIES: " ${GTK3_LIBRARIES} )
ENDIF ( GTK3_FOUND)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/includes")

add_executable (LDMicro ldmicro.cpp miscutil.cpp draw_outputdev.cpp)

install (TARGETS LDMicro DESTINATION bin)
