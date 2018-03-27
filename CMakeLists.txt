cmake_minimum_required(VERSION 2.8)
project(audio-test CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(lib/ddui)
add_subdirectory(lib/libsoundio)

list(APPEND ASSETS
    ${ddui_ASSETS}
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFRegular.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFRegularItalic.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFBold.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFBoldItalic.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFMedium.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFMediumItalic.ttf
)

add_executable(audio-test MACOSX_BUNDLE WIN32 ${SOURCES} ${ASSETS})

include_directories(lib/libsoundio)

if(APPLE)

    set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.icns)
    set_source_files_properties(${ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    
    set_target_properties(audio-test PROPERTIES
        MACOSX_BUNDLE_ICON_FILE icon.icns
        MACOSX_BUNDLE_INFO_PLIST ${ddui_MACOSX_BUNDLE}
        RESOURCE ${ICON}
    )
    
    set_source_files_properties(
        ${ASSETS}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources/assets
    )
    set(MACOSX_BUNDLE_BUNDLE_NAME "Audio Test")
        
else()

    file(COPY ${ASSETS} DESTINATION "assets/")

endif()

get_directory_property(LIBSOUNDIO_LIBS DIRECTORY lib/libsoundio DEFINITION LIBSOUNDIO_LIBS)

target_link_libraries(audio-test
    ddui
    libsoundio_static
    ${LIBSOUNDIO_LIBS}
)
