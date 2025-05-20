#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgDocument.h"
#include "../../src/CoreSvgEngine/SvgShapes.h"
#include "../../src/CoreSvgEngine/SvgText.h"
#include <iostream>
#include <sstream>
#include <string>

class SvgDocumentTest : public ::testing::Test {
protected:
    void SetUp() override {
        doc = new SvgDocument(800, 600);
    }

    void TearDown() override {
        delete doc;
    }

    SvgDocument* doc;
};

// 基本SVG解析测试
TEST_F(SvgDocumentTest, BasicSvgParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<rect width=\"100%\" height=\"100%\" fill=\"rgb(200,200,200)\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    EXPECT_EQ(400, doc->getWidth());
    EXPECT_EQ(300, doc->getHeight());
    
    // 检查背景色是否被正确解析
    Color bg = doc->getBackgroundColor();
    EXPECT_EQ(200, bg.r);
    EXPECT_EQ(200, bg.g);
    EXPECT_EQ(200, bg.b);
    EXPECT_EQ(255, bg.alpha);
}

// 测试解析SVG线条元素
TEST_F(SvgDocumentTest, LineParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<line id=\"test-line\" x1=\"10\" y1=\"20\" x2=\"100\" y2=\"200\" "
                             "stroke=\"rgb(255,0,0)\" stroke-width=\"3.5\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    // 检查元素数量
    EXPECT_EQ(1, doc->getElements().size());
    
    // 获取解析的元素
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Line, elem->getType());
    
    // 转换为具体类型
    SvgLine* line = dynamic_cast<SvgLine*>(elem);
    ASSERT_NE(nullptr, line);
    
    // 验证属性
    EXPECT_EQ("test-line", line->getID());
    EXPECT_EQ(10.0, line->getP1().x);
    EXPECT_EQ(20.0, line->getP1().y);
    EXPECT_EQ(100.0, line->getP2().x);
    EXPECT_EQ(200.0, line->getP2().y);
    
    // 验证样式属性
    EXPECT_EQ(255, line->getStrokeColor().r);
    EXPECT_EQ(0, line->getStrokeColor().g);
    EXPECT_EQ(0, line->getStrokeColor().b);
    EXPECT_EQ(3.5, line->getStrokeWidth());
}

// 测试解析SVG矩形元素
TEST_F(SvgDocumentTest, RectangleParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<rect id=\"test-rect\" x=\"50\" y=\"50\" width=\"200\" height=\"100\" "
                             "fill=\"rgb(0,0,255)\" stroke=\"rgb(0,0,0)\" rx=\"5\" ry=\"5\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Rectangle, elem->getType());
    
    SvgRectangle* rect = dynamic_cast<SvgRectangle*>(elem);
    ASSERT_NE(nullptr, rect);
    
    EXPECT_EQ("test-rect", rect->getID());
    EXPECT_EQ(50.0, rect->getTopLeft().x);
    EXPECT_EQ(50.0, rect->getTopLeft().y);
    EXPECT_EQ(200.0, rect->getWidth());
    EXPECT_EQ(100.0, rect->getHeight());
    EXPECT_EQ(5.0, rect->getRx());
    EXPECT_EQ(5.0, rect->getRy());
    
    EXPECT_EQ(0, rect->getFillColor().r);
    EXPECT_EQ(0, rect->getFillColor().g);
    EXPECT_EQ(255, rect->getFillColor().b);
}

// 测试解析SVG圆形元素
TEST_F(SvgDocumentTest, CircleParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<circle id=\"test-circle\" cx=\"150\" cy=\"150\" r=\"80\" "
                             "fill=\"rgb(255,255,0)\" stroke=\"rgb(128,128,0)\" stroke-width=\"2\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Circle, elem->getType());
    
    SvgCircle* circle = dynamic_cast<SvgCircle*>(elem);
    ASSERT_NE(nullptr, circle);
    
    EXPECT_EQ("test-circle", circle->getID());
    EXPECT_EQ(150.0, circle->getCenter().x);
    EXPECT_EQ(150.0, circle->getCenter().y);
    EXPECT_EQ(80.0, circle->getRadius());
    
    EXPECT_EQ(255, circle->getFillColor().r);
    EXPECT_EQ(255, circle->getFillColor().g);
    EXPECT_EQ(0, circle->getFillColor().b);
    EXPECT_EQ(128, circle->getStrokeColor().r);
    EXPECT_EQ(128, circle->getStrokeColor().g);
    EXPECT_EQ(0, circle->getStrokeColor().b);
    EXPECT_EQ(2.0, circle->getStrokeWidth());
}

// 测试解析SVG椭圆元素
TEST_F(SvgDocumentTest, EllipseParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<ellipse id=\"test-ellipse\" cx=\"200\" cy=\"100\" rx=\"150\" ry=\"75\" "
                             "fill=\"rgb(200,100,50)\" stroke=\"rgb(0,0,0)\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Ellipse, elem->getType());
    
    SvgEllipse* ellipse = dynamic_cast<SvgEllipse*>(elem);
    ASSERT_NE(nullptr, ellipse);
      EXPECT_EQ("test-ellipse", ellipse->getID());
    EXPECT_EQ(200.0, ellipse->getCenter().x);
    EXPECT_EQ(100.0, ellipse->getCenter().y);
    EXPECT_EQ(150.0, ellipse->getRx());
    EXPECT_EQ(75.0, ellipse->getRy());
    
    EXPECT_EQ(200, ellipse->getFillColor().r);
    EXPECT_EQ(100, ellipse->getFillColor().g);
    EXPECT_EQ(50, ellipse->getFillColor().b);
}

// 测试解析SVG多边形元素
TEST_F(SvgDocumentTest, PolygonParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<polygon id=\"test-polygon\" points=\"100,10 40,198 190,78 10,78 160,198\" "
                             "fill=\"rgb(50,200,50)\" stroke=\"rgb(0,100,0)\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Polygon, elem->getType());
    
    SvgPolygon* polygon = dynamic_cast<SvgPolygon*>(elem);
    ASSERT_NE(nullptr, polygon);
    
    EXPECT_EQ("test-polygon", polygon->getID());
    const std::vector<Point>& points = polygon->getPoints();
    ASSERT_EQ(5, points.size());
    
    // 检查点坐标
    EXPECT_EQ(100.0, points[0].x);
    EXPECT_EQ(10.0, points[0].y);
    EXPECT_EQ(40.0, points[1].x);
    EXPECT_EQ(198.0, points[1].y);
    EXPECT_EQ(190.0, points[2].x);
    EXPECT_EQ(78.0, points[2].y);
    
    EXPECT_EQ(50, polygon->getFillColor().r);
    EXPECT_EQ(200, polygon->getFillColor().g);
    EXPECT_EQ(50, polygon->getFillColor().b);
    
    EXPECT_EQ(0, polygon->getStrokeColor().r);
    EXPECT_EQ(100, polygon->getStrokeColor().g);
    EXPECT_EQ(0, polygon->getStrokeColor().b);
}

// 测试解析SVG折线元素
TEST_F(SvgDocumentTest, PolylineParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<polyline id=\"test-polyline\" points=\"20,20 40,25 60,40 80,120 120,140 200,180\" "
                             "fill=\"none\" stroke=\"rgb(255,0,255)\" stroke-width=\"3\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Polyline, elem->getType());
    
    SvgPolyline* polyline = dynamic_cast<SvgPolyline*>(elem);
    ASSERT_NE(nullptr, polyline);
    
    EXPECT_EQ("test-polyline", polyline->getID());
    const std::vector<Point>& points = polyline->getPoints();
    ASSERT_EQ(6, points.size());
    
    // 检查点坐标
    EXPECT_EQ(20.0, points[0].x);
    EXPECT_EQ(20.0, points[0].y);
    EXPECT_EQ(40.0, points[1].x);
    EXPECT_EQ(25.0, points[1].y);
    
    // 检查填充颜色是否为"none"（alpha = 0）
    EXPECT_EQ(0, polyline->getFillColor().alpha);
    
    // 检查描边颜色
    EXPECT_EQ(255, polyline->getStrokeColor().r);
    EXPECT_EQ(0, polyline->getStrokeColor().g);
    EXPECT_EQ(255, polyline->getStrokeColor().b);
    EXPECT_EQ(3.0, polyline->getStrokeWidth());
}

// 测试解析SVG文本元素
TEST_F(SvgDocumentTest, TextParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<text id=\"test-text\" x=\"50\" y=\"80\" font-family=\"Arial\" font-size=\"24\" "
                             "fill=\"rgb(0,0,0)\">Hello SVG</text>"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    EXPECT_EQ(SvgElementType::Text, elem->getType());
    
    SvgText* text = dynamic_cast<SvgText*>(elem);
    ASSERT_NE(nullptr, text);
    
    EXPECT_EQ("test-text", text->getID());
    EXPECT_EQ(50.0, text->getPosition().x);
    EXPECT_EQ(80.0, text->getPosition().y);
    EXPECT_EQ("Arial", text->getFontFamily());
    EXPECT_EQ(24.0, text->getFontSize());
    EXPECT_EQ("Hello SVG", text->getTextContent());
    
    EXPECT_EQ(0, text->getFillColor().r);
    EXPECT_EQ(0, text->getFillColor().g);
    EXPECT_EQ(0, text->getFillColor().b);
}

// 测试自定义属性解析
TEST_F(SvgDocumentTest, CustomAttributesParsing) {
    std::string svgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<rect id=\"test-custom\" x=\"10\" y=\"10\" width=\"100\" height=\"50\" "
                             "fill=\"rgb(255,0,0)\" data-custom-str=\"test\" data-custom-num=\"42.5\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgContent));
    
    ASSERT_EQ(1, doc->getElements().size());
    SvgElement* elem = doc->getElements()[0];
    
    // 检查自定义字符串属性
    auto customStrAttr = elem->getAttribute("data-custom-str");
    ASSERT_FALSE(customStrAttr.valueless_by_exception());
    EXPECT_EQ("test", std::get<std::string>(customStrAttr));
    
    // 检查自定义数值属性
    auto customNumAttr = elem->getAttribute("data-custom-num");
    ASSERT_FALSE(customNumAttr.valueless_by_exception());
    EXPECT_DOUBLE_EQ(42.5, std::get<double>(customNumAttr));
}

// 测试错误处理 - 无效SVG
TEST_F(SvgDocumentTest, InvalidSvgHandling) {
    std::string invalidSvg = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                             "<invalid-tag />"
                             "<rect x=\"10\" y=\"10\" width=\"100\" height=\"50\" />"
                             "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(invalidSvg));
    // 期望有一个矩形被解析，invalid-tag被忽略
    EXPECT_EQ(1, doc->getElements().size());
    EXPECT_EQ(SvgElementType::Rectangle, doc->getElements()[0]->getType());
}

// 测试错误处理 - 格式完全错误的SVG
TEST_F(SvgDocumentTest, MalformedSvgHandling) {
    std::string malformedSvg = "<not a valid svg document>";
    
    EXPECT_FALSE(doc->parseSvgContent(malformedSvg));
    EXPECT_EQ(0, doc->getElements().size());
}

// 测试错误处理 - 缺少属性的元素
TEST_F(SvgDocumentTest, MissingAttributesHandling) {
    std::string svgWithMissingAttrs = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                                     "<rect width=\"100\" height=\"50\" />"  // 缺少 x, y
                                     "<circle r=\"50\" />"                  // 缺少 cx, cy
                                     "</svg>";
    
    EXPECT_TRUE(doc->parseSvgContent(svgWithMissingAttrs));
    
    // 期望两个元素都被解析，只是使用了默认值
    EXPECT_EQ(2, doc->getElements().size());
      auto rect = dynamic_cast<SvgRectangle*>(doc->getElements()[0]);
    ASSERT_NE(nullptr, rect);
    EXPECT_EQ(0.0, rect->getTopLeft().x); // 期望使用默认值0
    EXPECT_EQ(0.0, rect->getTopLeft().y);
    EXPECT_EQ(100.0, rect->getWidth());
    EXPECT_EQ(50.0, rect->getHeight());
    
    auto circle = dynamic_cast<SvgCircle*>(doc->getElements()[1]);
    ASSERT_NE(nullptr, circle);
    EXPECT_EQ(0.0, circle->getCenter().x); // 期望使用默认值0
    EXPECT_EQ(0.0, circle->getCenter().y);
    EXPECT_EQ(50.0, circle->getRadius());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
