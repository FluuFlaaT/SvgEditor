add_executable(svg_document_tests SvgDocumentTest.cpp)

# 链接测试与依赖库
target_link_libraries(svg_document_tests PRIVATE 
    CoreSvgEngine 
    GTest::gtest 
    GTest::gtest_main
)

# 添加到测试
include(GoogleTest)
gtest_discover_tests(svg_document_tests)
