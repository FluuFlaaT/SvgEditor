add_executable(CoreSvgEngineTest main.cpp SvgDocumentTest.cpp SvgParserTest.cpp)

# 链接测试与依赖库
target_link_libraries(CoreSvgEngineTest PRIVATE 
    CoreSvgEngine 
    GTest::gtest 
    GTest::gtest_main
)

# 添加到测试
include(GoogleTest)
gtest_discover_tests(CoreSvgEngineTest)
