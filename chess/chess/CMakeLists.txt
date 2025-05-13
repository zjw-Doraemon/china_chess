cmake_minimum_required(VERSION 3.5)

project(chess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)
# 添加头文件搜索路径
include_directories(
    ${PROJECT_SOURCE_DIR}
)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        chessbase.h chessbase.cpp
        chessboard.h chessboard.cpp
        chessfactory.h chessfactory.cpp
        global.h global.cpp
        test.h test.cpp
        chessboardui.h chessboardui.cpp
        welcome.h welcome.cpp welcome.ui
        custom_widget.h custom_widget.cpp
        board_control.h board_control.cpp board_control.ui
        http.h http.cpp
        register_dialog.h register_dialog.cpp register_dialog.ui
        logindialog.h logindialog.cpp logindialog.ui
        tcp.h tcp.cpp
        friend_list.h friend_list.cpp friend_list.ui
        friend_item.h friend_item.cpp friend_item.ui
        user_data.h user_data.cpp
        new_chess_board_ui.h new_chess_board_ui.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chess
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        picture.qrc
        web_board_control.h web_board_control.cpp web_board_control.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chess APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chess SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chess
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(chess PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.chess)
endif()
set_target_properties(chess PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE false  # true ��gui����  false ����̨ gui һ������
)

include(GNUInstallDirs)
install(TARGETS chess
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chess)
endif()

# 复制配置文件
add_custom_command(
    TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/config.json
            $<TARGET_FILE_DIR:chess>/config.json
    COMMENT "Updating config.json if different"
)

add_custom_command(
    TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/ChessImage
            $<TARGET_FILE_DIR:chess>/ChessImage
    COMMENT "Updating ChessImage directory if different"
)

add_compile_options(/wd4828)

# 忽略编码警告
target_compile_options(chess PRIVATE /wd4828) 