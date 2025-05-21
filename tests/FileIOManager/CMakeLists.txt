add_executable(FileIOManagerTest FileIOManagerTest.cpp)

# 链接测试与依赖库
target_link_libraries(FileIOManagerTest PRIVATE 
    FileIOManager
    CoreSvgEngine
    GTest::gtest 
    GTest::gtest_main
    Qt5::Core
    Qt5::Gui
)

# 添加到测试
include(GoogleTest)
gtest_discover_tests(FileIOManagerTest)
