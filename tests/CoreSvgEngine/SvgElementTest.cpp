#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgElement.h"
#include "../../src/CoreSvgEngine/SvgShapes.h"

class SvgElementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 使用Line作为具体的SvgElement实现类进行测试
        line = new SvgLine({10, 20}, {100, 200});
    }

    void TearDown() override {
        delete line;
    }

    SvgLine* line;
};

// 测试基本属性设置和获取
TEST_F(SvgElementTest, BasicProperties) {
    // 设置ID
    line->setID("test-line-element");
    EXPECT_EQ("test-line-element", line->getID());
    
    // 设置描边颜色
    Color strokeColor = {255, 0, 0, 255}; // 红色
    line->setStrokeColor(strokeColor);
    
    Color retrievedStrokeColor = line->getStrokeColor();
    EXPECT_EQ(255, retrievedStrokeColor.r);
    EXPECT_EQ(0, retrievedStrokeColor.g);
    EXPECT_EQ(0, retrievedStrokeColor.b);
    EXPECT_EQ(255, retrievedStrokeColor.alpha);
    
    // 设置描边宽度
    line->setStrokeWidth(3.5);
    EXPECT_EQ(3.5, line->getStrokeWidth());
    
    // 设置填充颜色
    Color fillColor = {0, 0, 255, 128}; // 半透明蓝色
    line->setFillColor(fillColor);
    
    Color retrievedFillColor = line->getFillColor();
    EXPECT_EQ(0, retrievedFillColor.r);
    EXPECT_EQ(0, retrievedFillColor.g);
    EXPECT_EQ(255, retrievedFillColor.b);
    EXPECT_EQ(128, retrievedFillColor.alpha);
    
    // 设置透明度
    line->setOpacity(0.5);
    EXPECT_EQ(0.5, line->getOpacity());
}

// 测试自定义属性设置和获取
TEST_F(SvgElementTest, CustomAttributes) {
    // 设置字符串属性
    line->setAttribute("data-custom-str", std::string("test-value"));
    
    // 设置数值属性
    line->setAttribute("data-custom-num", 42.5);
    
    // 设置整数属性
    line->setAttribute("data-custom-int", 100);
    
    // 获取并验证属性
    auto strAttr = line->getAttribute("data-custom-str");
    ASSERT_FALSE(strAttr.valueless_by_exception());
    EXPECT_EQ("test-value", std::get<std::string>(strAttr));
    
    auto numAttr = line->getAttribute("data-custom-num");
    ASSERT_FALSE(numAttr.valueless_by_exception());
    EXPECT_DOUBLE_EQ(42.5, std::get<double>(numAttr));
    
    auto intAttr = line->getAttribute("data-custom-int");
    ASSERT_FALSE(intAttr.valueless_by_exception());
    EXPECT_EQ(100, std::get<int>(intAttr));
      // 测试获取不存在的属性
    auto nonExistentAttr = line->getAttribute("non-existent");
    // 由于技术限制，我们的变体不能创建 valueless_by_exception 状态
    // 所以检查它是否是默认构造的字符串（空字符串）
    ASSERT_FALSE(nonExistentAttr.valueless_by_exception());
    EXPECT_EQ("", std::get<std::string>(nonExistentAttr));
    
    // 覆盖现有属性
    line->setAttribute("data-custom-str", std::string("new-value"));
    auto updatedAttr = line->getAttribute("data-custom-str");
    ASSERT_FALSE(updatedAttr.valueless_by_exception());
    EXPECT_EQ("new-value", std::get<std::string>(updatedAttr));
}

// 测试SVG字符串生成
TEST_F(SvgElementTest, SvgStringGeneration) {
    // 设置各种属性
    line->setID("test-line");
    line->setStrokeColor({255, 0, 0, 255});
    line->setStrokeWidth(2.5);
    line->setOpacity(0.8);
    
    // 获取SVG字符串表示
    std::string svgString = line->toSvgString();
    
    // 验证字符串包含所有必要的属性
    EXPECT_NE(std::string::npos, svgString.find("<line"));
    EXPECT_NE(std::string::npos, svgString.find("id=\"test-line\""));
    EXPECT_NE(std::string::npos, svgString.find("x1=\"10\""));
    EXPECT_NE(std::string::npos, svgString.find("y1=\"20\""));
    EXPECT_NE(std::string::npos, svgString.find("x2=\"100\""));
    EXPECT_NE(std::string::npos, svgString.find("y2=\"200\""));
    EXPECT_NE(std::string::npos, svgString.find("stroke=\"rgb(255,0,0)\""));
    EXPECT_NE(std::string::npos, svgString.find("stroke-width=\"2.5\""));
    EXPECT_NE(std::string::npos, svgString.find("opacity=\"0.8\""));
}

// 测试通用属性字符串生成
TEST_F(SvgElementTest, CommonAttributesString) {
    // 设置各种通用属性
    line->setID("test-element");
    line->setStrokeColor({0, 128, 0, 255}); // 绿色
    line->setStrokeWidth(1.5);
    line->setFillColor({255, 255, 0, 255}); // 黄色
    line->setOpacity(0.7);
    
    // 获取通用属性字符串
    std::string attrString = line->getCommonAttributesString();
    
    // 验证字符串包含所有设置的属性
    EXPECT_NE(std::string::npos, attrString.find("id=\"test-element\""));
    EXPECT_NE(std::string::npos, attrString.find("stroke=\"rgb(0,128,0)\""));
    EXPECT_NE(std::string::npos, attrString.find("stroke-width=\"1.5\""));
    EXPECT_NE(std::string::npos, attrString.find("fill=\"rgb(255,255,0)\""));
    EXPECT_NE(std::string::npos, attrString.find("opacity=\"0.7\""));
}
