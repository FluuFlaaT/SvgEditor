#define _USE_MATH_DEFINES
#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgShapes.h"
#include <cmath>

class SvgSpecialShapesTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// 测试五边形
TEST_F(SvgSpecialShapesTest, PentagonTest) {
    // 创建一个以(100,100)为中心、半径为50的五边形
    SvgPentagon pentagon(Point{100, 100}, 50);
    pentagon.setID("test-pentagon");
    pentagon.setFillColor({50, 150, 50, 255});
    
    // 检查类型
    EXPECT_EQ(SvgElementType::Pentagon, pentagon.getType());
    
    // 检查ID
    EXPECT_EQ("test-pentagon", pentagon.getID());
    
    // 检查点的数量
    EXPECT_EQ(5, pentagon.getPoints().size());
    
    // 验证点的位置 - 五边形的顶点应该在中心点周围均匀分布
    const std::vector<Point>& points = pentagon.getPoints();
    
    // 检查点是否位于圆上（所有点到中心的距离应约等于半径）
    for (const auto& point : points) {
        double distance = std::sqrt(std::pow(point.x - 100, 2) + std::pow(point.y - 100, 2));
        EXPECT_NEAR(50.0, distance, 0.01); // 允许0.01的误差
    }
    
    // 验证角度是均匀分布的（相邻点之间的夹角应为2π/5）
    double expectedAngleDiff = 2 * M_PI / 5;
    for (size_t i = 0; i < points.size(); i++) {
        Point p1 = points[i];
        Point p2 = points[(i + 1) % points.size()];
        
        // 计算两点到中心的角度
        double angle1 = std::atan2(p1.y - 100, p1.x - 100);
        double angle2 = std::atan2(p2.y - 100, p2.x - 100);
        
        // 计算角度差（需要处理角度环绕）
        double angleDiff = std::abs(angle1 - angle2);
        if (angleDiff > M_PI) {
            angleDiff = 2 * M_PI - angleDiff;
        }
        
        EXPECT_NEAR(expectedAngleDiff, angleDiff, 0.01); // 允许0.01的误差
    }
    
    // 检查填充颜色
    EXPECT_EQ(50, pentagon.getFillColor().r);
    EXPECT_EQ(150, pentagon.getFillColor().g);
    EXPECT_EQ(50, pentagon.getFillColor().b);
    EXPECT_EQ(255, pentagon.getFillColor().alpha);
    
    // 检查SVG字符串包含正确的多边形点
    std::string svgString = pentagon.toSvgString();
    EXPECT_NE(std::string::npos, svgString.find("<polygon"));
    EXPECT_NE(std::string::npos, svgString.find("id=\"test-pentagon\""));
    EXPECT_NE(std::string::npos, svgString.find("fill=\"rgb(50,150,50)\""));
}

// 测试六边形
TEST_F(SvgSpecialShapesTest, HexagonTest) {
    // 创建一个以(150,150)为中心、半径为75的六边形
    SvgHexagon hexagon(Point{150, 150}, 75);
    hexagon.setID("test-hexagon");
    hexagon.setFillColor({200, 100, 50, 255});
    hexagon.setStrokeColor({100, 50, 25, 255});
    hexagon.setStrokeWidth(2.5);
    
    // 检查类型
    EXPECT_EQ(SvgElementType::Hexagon, hexagon.getType());
    
    // 检查ID和样式属性
    EXPECT_EQ("test-hexagon", hexagon.getID());
    EXPECT_EQ(200, hexagon.getFillColor().r);
    EXPECT_EQ(100, hexagon.getFillColor().g);
    EXPECT_EQ(50, hexagon.getFillColor().b);
    EXPECT_EQ(100, hexagon.getStrokeColor().r);
    EXPECT_EQ(50, hexagon.getStrokeColor().g);
    EXPECT_EQ(25, hexagon.getStrokeColor().b);
    EXPECT_EQ(2.5, hexagon.getStrokeWidth());
    
    // 检查点的数量
    EXPECT_EQ(6, hexagon.getPoints().size());
    
    // 验证点的位置
    const std::vector<Point>& points = hexagon.getPoints();
    
    // 检查点是否位于圆上
    for (const auto& point : points) {
        double distance = std::sqrt(std::pow(point.x - 150, 2) + std::pow(point.y - 150, 2));
        EXPECT_NEAR(75.0, distance, 0.01);
    }
    
    // 验证角度是均匀分布的（相邻点之间的夹角应为2π/6）
    double expectedAngleDiff = 2 * M_PI / 6;
    for (size_t i = 0; i < points.size(); i++) {
        Point p1 = points[i];
        Point p2 = points[(i + 1) % points.size()];
        
        double angle1 = std::atan2(p1.y - 150, p1.x - 150);
        double angle2 = std::atan2(p2.y - 150, p2.x - 150);
        
        double angleDiff = std::abs(angle1 - angle2);
        if (angleDiff > M_PI) {
            angleDiff = 2 * M_PI - angleDiff;
        }
        
        EXPECT_NEAR(expectedAngleDiff, angleDiff, 0.01);
    }
    
    // 检查SVG字符串
    std::string svgString = hexagon.toSvgString();
    EXPECT_NE(std::string::npos, svgString.find("<polygon"));
    EXPECT_NE(std::string::npos, svgString.find("id=\"test-hexagon\""));
    EXPECT_NE(std::string::npos, svgString.find("fill=\"rgb(200,100,50)\""));
    EXPECT_NE(std::string::npos, svgString.find("stroke=\"rgb(100,50,25)\""));
    EXPECT_NE(std::string::npos, svgString.find("stroke-width=\"2.5\""));
}

// 测试五角星
TEST_F(SvgSpecialShapesTest, StarTest) {
    // 创建一个以(200,200)为中心、外半径为100、内半径为50的五角星
    SvgStar star(Point{200, 200}, 100, 50);
    star.setID("test-star");
    star.setFillColor({255, 255, 0, 255}); // 黄色
    
    // 检查类型
    EXPECT_EQ(SvgElementType::Star, star.getType());
    
    // 检查ID
    EXPECT_EQ("test-star", star.getID());
    
    // 检查点的数量 - 五角星应该有10个点（内外交替）
    EXPECT_EQ(10, star.getPoints().size());
    
    // 验证点的位置
    const std::vector<Point>& points = star.getPoints();
    
    // 检查外部点是否位于外圆上，内部点是否位于内圆上（交替）
    for (size_t i = 0; i < points.size(); i++) {
        const Point& point = points[i];
        double distance = std::sqrt(std::pow(point.x - 200, 2) + std::pow(point.y - 200, 2));
        
        if (i % 2 == 0) { // 外部点
            EXPECT_NEAR(100.0, distance, 0.01);
        } else { // 内部点
            EXPECT_NEAR(50.0, distance, 0.01);
        }
    }
    
    // 检查填充颜色
    EXPECT_EQ(255, star.getFillColor().r);
    EXPECT_EQ(255, star.getFillColor().g);
    EXPECT_EQ(0, star.getFillColor().b);
    
    // 检查SVG字符串
    std::string svgString = star.toSvgString();
    EXPECT_NE(std::string::npos, svgString.find("<polygon"));
    EXPECT_NE(std::string::npos, svgString.find("id=\"test-star\""));
    EXPECT_NE(std::string::npos, svgString.find("fill=\"rgb(255,255,0)\""));
}
