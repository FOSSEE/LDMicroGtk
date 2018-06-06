cmake_minimum_required(VERSION 2.6)

project(LDMicro)

IF(WIN32)
    MESSAGE( FATAL_ERROR "Cannot build for windows, exiting" )
    add_definitions(-D__WIN32__)
ENDIF(WIN32)

IF (MSVC)
    MESSAGE( FATAL_ERROR "Cannot build for MacOS, exiting" )
    add_definitions(-D__MSVC__)
ENDIF (MSVC)

IF(UNIX)
    add_definitions(-D__UNIX__)

    MESSAGE( STATUS "Performing system check.." )
    MESSAGE( STATUS "Identifing bitness of the platform.." )
    add_definitions(-D__UNIX32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-D__UNIX64)
        MESSAGE( STATUS "Bitness of the platform: " 64)
    else()
        MESSAGE( STATUS "Bitness of the platform: " 36)
    endif()
    MESSAGE( STATUS "Performing system check - done" )

    MESSAGE ( STATUS "Searching for GTK+3.0..." )
    find_package (PkgConfig REQUIRED)
    pkg_check_modules (GTK3 REQUIRED gtk+-3.0)

    ## Version control
    set (LDMicro_VERSION_MAJOR 1)
    set (LDMicro_VERSION_MINOR 0)

    ## configure a header file to pass some of the CMake settings
    ## to the source code
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/linuxUI")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/freezeLD")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/includes")
    set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/includes")
    CONFIGURE_FILE (
        "${PROJECT_INCLUDE_DIR}/ldmicroVC.h.in"
        "${PROJECT_INCLUDE_DIR}/ldmicroVC.h"
    )
    MESSAGE ( STATUS " PROJECT_INCLUDE_DIR: " ${PROJECT_INCLUDE_DIR} )

    ## Add GTK3 include files if GTK3 is found
    IF ( GTK3_FOUND )
        include_directories (${GTK3_INCLUDE_DIRS})
        link_directories (${GTK3_LIBRARY_DIRS})
        add_definitions (${GTK3_CFLAGS_OTHER})
        link_libraries (${GTK3_LIBRARIES})
        MESSAGE ( STATUS " GTK3_INCLUDE_DIR: " ${GTK3_INCLUDE_DIRS} )
        MESSAGE ( STATUS " GTK3_LIBRARIES: " ${GTK3_LIBRARIES} )
    ENDIF ( GTK3_FOUND )
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/linuxUI")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/freezeLD")
    
    ## Dummy compile and install to test linuxUI
    ## to compile LDmicro uncomment the below 2 line2
    set (COMPILE_CPP_SOURCES arduino.cpp
                            avr.cpp
                            pic16.cpp
                            interpreted.cpp
                            ansic.cpp
                            compilecommon.cpp
                            intcode.cpp
                            lang.cpp
                            miscutil.cpp
                            iolist.cpp
                            confdialog.cpp
                            lutdialog.cpp
                            resetdialog.cpp
                            simpledialog.cpp
                            coildialog.cpp
                            contactsdialog.cpp
                            commentdialog.cpp
                            simulate.cpp
                            loadsave.cpp
                            undoredo.cpp
                            circuit.cpp
                            draw_outputdev.cpp
                            draw.cpp
                            schematic.cpp
                            helpdialog.cpp
                            maincontrols.cpp
                            ldmicro.cpp)
    
    add_executable (LDMicro ${COMPILE_CPP_SOURCES})
    install (TARGETS LDMicro DESTINATION bin)
    target_link_libraries (LDMicro LinuxUI)
    target_link_libraries (LDMicro FreezeLD)
    #add_executable (testMain testMain.cpp)
    #install (TARGETS testMain DESTINATION bin)
ENDIF(UNIX)
