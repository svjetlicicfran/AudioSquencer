cmake_minimum_required(VERSION 3.19)
project("audioSequencerStructured" LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core Widgets Multimedia Network Sql
)

qt_standard_project_setup()

qt_add_executable("audioSequencerStructured")

target_sources("audioSequencerStructured" PRIVATE
    # source files
    src/main.cpp
    src/mainwindow.cpp
    src/stepbutton.cpp
    src/track.cpp
    src/logindialog.cpp
    src/waveformworker.cpp
    src/waveformwidget.cpp
    src/registerdialog.cpp
    src/browsesoundsdialog.cpp
    src/soundpreviewe.cpp
    src/freesoundbrowser.cpp
    src/databaseeditor.cpp
    src/logseditor.cpp
    #src/dialog.cpp
    #src/scrollarea.cpp
    #src/steptask.cpp
    #src/sum.cpp

    # headers
    include/mainwindow.h
    include/stepbutton.h
    include/track.h
    include/constanst.h
    include/logindialog.h
    include/waveformworker.h
    include/waveformwidget.h
    include/registerdialog.h
    include/browsesoundsdialog.h
    include/soundpreviewe.h
    include/freesoundbrowser.h
    include/databaseeditor.h
    include/logseditor.h
    #include/dialog.h
    #include/scrollarea.h
    #include/steptask.h
    #include/sum.h
    include/xmlfilereader.h
    include/xmlwriter.h

    # UI files
    src/mainwindow.ui
    src/logindialog.ui
    src/registerdialog.ui
    src/browsesoundsdialog.ui
    src/freesoundbrowser.ui
    src/databaseeditor.ui
    src/logseditor.ui
    #src/dialog.ui
    #src/scrollarea.ui
)

target_include_directories("audioSequencerStructured"
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# static library import
add_library("XMLFileReader" STATIC IMPORTED)
set_target_properties("XMLFileReader" PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libXMLFileReader.a
)

# dynamic library import
add_library("XMLFileSaver" SHARED IMPORTED)
set_target_properties("XMLFileSaver" PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libXMLFileSaver.so
)

# link stuff
target_link_libraries("audioSequencerStructured"
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
        Qt::Network
        Qt::Sql
        XMLFileReader
        XMLFileSaver
)

# rdpath emebed for shared libs
set_target_properties("audioSequencerStructured" PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib"
)

include(GNUInstallDirs)

install(TARGETS "audioSequencerStructured"
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET "audioSequencerStructured"
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
