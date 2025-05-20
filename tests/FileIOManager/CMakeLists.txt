add_executable(file_io_manager_tests FileIOManagerTest.cpp)

# 链接测试与依赖库
target_link_libraries(file_io_manager_tests PRIVATE 
    FileIOManager
    CoreSvgEngine
    GTest::gtest 
    GTest::gtest_main
    Qt5::Core
    Qt5::Gui
)

# 添加到测试
include(GoogleTest)
gtest_discover_tests(file_io_manager_tests)
