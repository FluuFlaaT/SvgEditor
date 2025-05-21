#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgText.h"

class SvgTextTest : public ::testing::Test {
protected:
    void SetUp() override {
        text = new SvgText({100, 100}, "Hello SVG");
    }

    void TearDown() override {
        delete text;
    }

    SvgText* text;
};

// 测试文本基本属性
TEST_F(SvgTextTest, BasicTextProperties) {
    // 检查默认属性
    EXPECT_EQ(SvgElementType::Text, text->getType());
    EXPECT_EQ("Hello SVG", text->getTextContent());
    EXPECT_EQ(100.0, text->getPosition().x);
    EXPECT_EQ(100.0, text->getPosition().y);
    
    // 修改文本内容
    text->setTextContent("Modified Text Content");
    EXPECT_EQ("Modified Text Content", text->getTextContent());
    
    // 修改位置
    text->setPosition({200, 150});
    EXPECT_EQ(200.0, text->getPosition().x);
    EXPECT_EQ(150.0, text->getPosition().y);
}

// 测试文本字体相关属性
TEST_F(SvgTextTest, FontProperties) {
    // 设置字体属性
    text->setFontFamily("Arial");
    text->setFontSize(24.0);
    
    // 验证字体属性
    EXPECT_EQ("Arial", text->getFontFamily());
    EXPECT_EQ(24.0, text->getFontSize());
}

// 测试文本颜色
TEST_F(SvgTextTest, TextColorProperties) {
    // 设置文本颜色
    text->setFillColor({50, 100, 150, 255});
    
    Color retrievedColor = text->getFillColor();
    EXPECT_EQ(50, retrievedColor.r);
    EXPECT_EQ(100, retrievedColor.g);
    EXPECT_EQ(150, retrievedColor.b);
}

// 移除不存在的文本对齐测试

// 测试SVG字符串生成
TEST_F(SvgTextTest, SvgStringGeneration) {
    // 设置各种属性
    text->setID("test-text");
    text->setFontFamily("Verdana");
    text->setFontSize(18.0);
    text->setFillColor({0, 0, 0, 255}); // 黑色
    
    // 获取SVG字符串
    std::string svgString = text->toSvgString();
    
    // 验证字符串包含所有设置的属性
    EXPECT_NE(std::string::npos, svgString.find("<text"));
    EXPECT_NE(std::string::npos, svgString.find("id=\"test-text\""));
    EXPECT_NE(std::string::npos, svgString.find("x=\"100\""));
    EXPECT_NE(std::string::npos, svgString.find("y=\"100\""));
    EXPECT_NE(std::string::npos, svgString.find("font-family=\"Verdana\""));
    EXPECT_NE(std::string::npos, svgString.find("font-size=\"18\""));
    EXPECT_NE(std::string::npos, svgString.find("fill=\"rgb(0,0,0)\""));
    EXPECT_NE(std::string::npos, svgString.find(">Hello SVG<"));
}

// 测试特殊文本内容处理
TEST_F(SvgTextTest, SpecialTextContent) {
    // 测试包含特殊字符的文本
    text->setTextContent("Special <characters> & \"quotes\" need escaping");
    
    std::string svgString = text->toSvgString();
    
    // 验证特殊字符被正确转义
    EXPECT_NE(std::string::npos, svgString.find("Special &lt;characters&gt; &amp; &quot;quotes&quot; need escaping"));
    
    // 测试空文本
    text->setTextContent("");
    EXPECT_EQ("", text->getTextContent());
    EXPECT_NE(std::string::npos, text->toSvgString().find("></text>"));
    
    // 测试多行文本
    text->setTextContent("Line 1\nLine 2\nLine 3");
    svgString = text->toSvgString();
    EXPECT_NE(std::string::npos, svgString.find("Line 1"));
    // 注意：多行文本的处理可能需要检查具体实现是如何处理的
}
