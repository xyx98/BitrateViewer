cmake_minimum_required(VERSION 3.19)
project(BitrateViewer LANGUAGES CXX)


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets WebEngineWidgets Concurrent)
qt_standard_project_setup()

qt_add_executable(BitrateViewer
    WIN32 MACOSX_BUNDLE
    config.h
    backend.h
    backend.cpp
    sha1.hpp
    plot.h
    plot.cpp
    ini.h
    ini.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

IF (WIN32)
    set(ffmpeg_dir ${PROJECT_SOURCE_DIR}/ffmpeg)
    include_directories(${ffmpeg_dir}/include)
    link_directories(${ffmpeg_dir}/lib)

    target_link_libraries(BitrateViewer
        PRIVATE
            Qt::Core
            Qt::Widgets
            Qt::WebEngineWidgets
            Qt::Concurrent
            ${ffmpeg_dir}/lib/avcodec.lib
            ${ffmpeg_dir}/lib/avformat.lib
            ${ffmpeg_dir}/lib/avutil.lib
        )
ELSE ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil)
    target_link_libraries(BitrateViewer
        PRIVATE
            Qt::Core
            Qt::Widgets
            Qt::WebEngineWidgets
            Qt::Concurrent
            PkgConfig::ffmpeg
        )
ENDIF ()


include(GNUInstallDirs)

install(TARGETS BitrateViewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET BitrateViewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
