set(TARGET_NAME Core)

set(HEADERS
    ShapeType.h
    CoreForwards.h
)

add_library(${TARGET_NAME} INTERFACE)

target_include_directories(${TARGET_NAME} INTERFACE
    ${CMAKE_SOURCE_DIR}/src
)


