list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wave_reader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wave_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_client.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_client.cpp
)
set(SOURCES ${SOURCES} PARENT_SCOPE)
