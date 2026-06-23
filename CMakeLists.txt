cmake_minimum_required(VERSION 3.16)

project(visual_studio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

qt_standard_project_setup(REQUIRES 6.10)

# qt_add_resources(RC_SOURCES fonts.qrc)

qt_add_executable(appvisual_studio
    main.cpp
    # ${RC_RESOURCES}
    fonts.qrc
    # ./Dependencies/STFT/stft.cpp
)

qt_add_qml_module(appvisual_studio
    URI visual_studio
    QML_FILES Main.qml
    SOURCES SCholder.h SCholder.cpp
    SOURCES youreyes.h youreyes.cpp
    QML_FILES SpectrumWindow.qml
    SOURCES shadermanager.h shadermanager.cpp
    SOURCES sampler.h sampler.cpp
    RESOURCES fonts.qrc
    QML_FILES ShaderWindow.qml
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appvisual_studio PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appvisual_studio
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appvisual_studio
    PRIVATE Qt6::Quick Qt6::OpenGL
)
target_link_libraries(appvisual_studio PRIVATE Qt6::Widgets)
target_link_libraries(appvisual_studio
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bin/libmySTFT.dll
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bin/sndfile.dll
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bin/libfftw3-3.dll
)

install(TARGETS appvisual_studio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
