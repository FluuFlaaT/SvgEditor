set(TARGET_NAME CoreSvgEngine)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

add_library(${TARGET_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/LoggingService # 添加 LoggingService 头文件目录
)

target_link_libraries(${TARGET_NAME} PRIVATE LoggingService) # 链接 LoggingService

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
