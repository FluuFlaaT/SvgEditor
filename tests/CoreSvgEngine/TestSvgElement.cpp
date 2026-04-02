#include <gtest/gtest.h>
#include <CoreSvgEngine/SvgShapes.h>

class SvgElementTest : public ::testing::Test {
protected:
};

TEST_F(SvgElementTest, SvgLineDefaultPoints) {
    SvgLine line;
    EXPECT_EQ(line.getP1().x, 0.0);
    EXPECT_EQ(line.getP1().y, 0.0);
    EXPECT_EQ(line.getP2().x, 0.0);
    EXPECT_EQ(line.getP2().y, 0.0);
}

TEST_F(SvgElementTest, SvgLineSetPoints) {
    SvgLine line;
    line.setP1({10, 20});
    line.setP2({30, 40});
    EXPECT_EQ(line.getP1().x, 10.0);
    EXPECT_EQ(line.getP1().y, 20.0);
    EXPECT_EQ(line.getP2().x, 30.0);
    EXPECT_EQ(line.getP2().y, 40.0);
}

TEST_F(SvgElementTest, SvgLineTypeIsLine) {
    SvgLine line;
    EXPECT_EQ(line.getType(), SvgElementType::Line);
}

TEST_F(SvgElementTest, SvgRectangleDefaultDimensions) {
    SvgRectangle rect;
    EXPECT_EQ(rect.getWidth(), 0.0);
    EXPECT_EQ(rect.getHeight(), 0.0);
}

TEST_F(SvgElementTest, SvgRectangleSetDimensions) {
    SvgRectangle rect;
    rect.setWidth(100);
    rect.setHeight(50);
    EXPECT_EQ(rect.getWidth(), 100.0);
    EXPECT_EQ(rect.getHeight(), 50.0);
}

TEST_F(SvgElementTest, SvgRectangleTypeIsRectangle) {
    SvgRectangle rect;
    EXPECT_EQ(rect.getType(), SvgElementType::Rectangle);
}

TEST_F(SvgElementTest, SvgEllipseDefaultRadii) {
    SvgEllipse ellipse;
    EXPECT_EQ(ellipse.getRx(), 0.0);
    EXPECT_EQ(ellipse.getRy(), 0.0);
}

TEST_F(SvgElementTest, SvgEllipseSetRadii) {
    SvgEllipse ellipse;
    ellipse.setRx(30);
    ellipse.setRy(20);
    EXPECT_EQ(ellipse.getRx(), 30.0);
    EXPECT_EQ(ellipse.getRy(), 20.0);
}

TEST_F(SvgElementTest, SvgEllipseTypeIsEllipse) {
    SvgEllipse ellipse;
    EXPECT_EQ(ellipse.getType(), SvgElementType::Ellipse);
}

TEST_F(SvgElementTest, SvgCircleDefaultRadius) {
    SvgCircle circle;
    EXPECT_EQ(circle.getRadius(), 0.0);
}

TEST_F(SvgElementTest, SvgCircleSetRadius) {
    SvgCircle circle;
    circle.setRadius(25);
    EXPECT_EQ(circle.getRadius(), 25.0);
}

TEST_F(SvgElementTest, SvgCircleTypeIsCircle) {
    SvgCircle circle;
    EXPECT_EQ(circle.getType(), SvgElementType::Circle);
}

TEST_F(SvgElementTest, SvgPolygonEmptyPoints) {
    SvgPolygon polygon;
    EXPECT_EQ(polygon.getPoints().size(), 0);
}

TEST_F(SvgElementTest, SvgPolygonAddPoint) {
    SvgPolygon polygon;
    polygon.addPoint({10, 20});
    EXPECT_EQ(polygon.getPoints().size(), 1);
    EXPECT_EQ(polygon.getPoints()[0].x, 10.0);
    EXPECT_EQ(polygon.getPoints()[0].y, 20.0);
}

TEST_F(SvgElementTest, SvgPolygonTypeIsPolygon) {
    SvgPolygon polygon;
    EXPECT_EQ(polygon.getType(), SvgElementType::Polygon);
}

TEST_F(SvgElementTest, SvgPentagonTypeIsPentagon) {
    SvgPentagon pentagon;
    EXPECT_EQ(pentagon.getType(), SvgElementType::Pentagon);
}

TEST_F(SvgElementTest, SvgHexagonTypeIsHexagon) {
    SvgHexagon hexagon;
    EXPECT_EQ(hexagon.getType(), SvgElementType::Hexagon);
}

TEST_F(SvgElementTest, SvgStarTypeIsStar) {
    SvgStar star;
    EXPECT_EQ(star.getType(), SvgElementType::Star);
}

TEST_F(SvgElementTest, SvgPolylineTypeIsPolyline) {
    SvgPolyline polyline;
    EXPECT_EQ(polyline.getType(), SvgElementType::Polyline);
}

TEST_F(SvgElementTest, SvgElementStrokeColor) {
    SvgRectangle rect;
    Color strokeColor = {255, 0, 0, 255};
    rect.setStrokeColor(strokeColor);
    EXPECT_EQ(rect.getStrokeColor().r, 255);
    EXPECT_EQ(rect.getStrokeColor().g, 0);
    EXPECT_EQ(rect.getStrokeColor().b, 0);
}

TEST_F(SvgElementTest, SvgElementFillColor) {
    SvgRectangle rect;
    Color fillColor = {0, 255, 0, 255};
    rect.setFillColor(fillColor);
    EXPECT_EQ(rect.getFillColor().r, 0);
    EXPECT_EQ(rect.getFillColor().g, 255);
    EXPECT_EQ(rect.getFillColor().b, 0);
}

TEST_F(SvgElementTest, SvgElementStrokeWidth) {
    SvgRectangle rect;
    rect.setStrokeWidth(2.5);
    EXPECT_EQ(rect.getStrokeWidth(), 2.5);
}

TEST_F(SvgElementTest, SvgElementOpacity) {
    SvgRectangle rect;
    rect.setOpacity(0.5);
    EXPECT_EQ(rect.getOpacity(), 0.5);
}

TEST_F(SvgElementTest, SvgElementID) {
    SvgRectangle rect;
    rect.setID("test_id");
    EXPECT_EQ(rect.getID(), "test_id");
}

TEST_F(SvgElementTest, SetStrokeWidthNegativeClampedToZero) {
    SvgRectangle rect;
    rect.setStrokeWidth(-5.0);
    EXPECT_EQ(rect.getStrokeWidth(), 0.0);
}

TEST_F(SvgElementTest, SetOpacityNegativeClampedToZero) {
    SvgRectangle rect;
    rect.setOpacity(-0.5);
    EXPECT_EQ(rect.getOpacity(), 0.0);
}

TEST_F(SvgElementTest, SetOpacityGreaterThanOneClampedToOne) {
    SvgRectangle rect;
    rect.setOpacity(1.5);
    EXPECT_EQ(rect.getOpacity(), 1.0);
}

TEST_F(SvgElementTest, GetCommonAttributesStringWithEmptyId) {
    SvgRectangle rect;
    rect.setID("");
    rect.setStrokeColor({0, 0, 0, 255});
    rect.setStrokeWidth(1.0);
    rect.setFillColor({255, 255, 255, 255});
    std::string attrs = rect.getCommonAttributesString();
    EXPECT_TRUE(attrs.find("id=") == std::string::npos);
    EXPECT_TRUE(attrs.find("stroke=") != std::string::npos);
}

TEST_F(SvgElementTest, GetCommonAttributesStringWithOpacity) {
    SvgRectangle rect;
    rect.setOpacity(0.5);
    std::string attrs = rect.getCommonAttributesString();
    EXPECT_TRUE(attrs.find("opacity=") != std::string::npos);
}

TEST_F(SvgElementTest, GetCommonAttributesStringWithTransparentFill) {
    SvgRectangle rect;
    rect.setFillColor({0, 0, 0, 0});
    std::string attrs = rect.getCommonAttributesString();
    EXPECT_TRUE(attrs.find("fill=\"none\"") != std::string::npos);
}

TEST_F(SvgElementTest, SvgRectangleNegativeWidthClamped) {
    SvgRectangle rect(Point{0, 0}, -100, 50);
    EXPECT_EQ(rect.getWidth(), 0.0);
}

TEST_F(SvgElementTest, SvgRectangleNegativeHeightClamped) {
    SvgRectangle rect(Point{0, 0}, 100, -50);
    EXPECT_EQ(rect.getHeight(), 0.0);
}

TEST_F(SvgElementTest, SvgRectangleToSvgStringWithRxRy) {
    SvgRectangle rect(Point{10, 20}, 100, 50, 5, 5);
    std::string svg = rect.toSvgString();
    EXPECT_TRUE(svg.find("rx=\"5\"") != std::string::npos);
    EXPECT_TRUE(svg.find("ry=\"5\"") != std::string::npos);
}

TEST_F(SvgElementTest, SvgRectangleSetRx) {
    SvgRectangle rect;
    rect.setRx(10);
    EXPECT_EQ(rect.getRx(), 10.0);
}

TEST_F(SvgElementTest, SvgRectangleSetRy) {
    SvgRectangle rect;
    rect.setRy(10);
    EXPECT_EQ(rect.getRy(), 10.0);
}

TEST_F(SvgElementTest, SvgRectangleSetTopLeft) {
    SvgRectangle rect;
    rect.setTopLeft({50, 60});
    EXPECT_EQ(rect.getTopLeft().x, 50.0);
    EXPECT_EQ(rect.getTopLeft().y, 60.0);
}

TEST_F(SvgElementTest, SvgCircleSetCenter) {
    SvgCircle circle;
    circle.setCenter({100, 200});
    EXPECT_EQ(circle.getCenter().x, 100.0);
    EXPECT_EQ(circle.getCenter().y, 200.0);
}

TEST_F(SvgElementTest, SvgCircleNegativeRadiusClamped) {
    SvgCircle circle(Point{0, 0}, -50);
    EXPECT_EQ(circle.getRadius(), 0.0);
}

TEST_F(SvgElementTest, SvgEllipseSetCenter) {
    SvgEllipse ellipse;
    ellipse.setCenter({100, 200});
    EXPECT_EQ(ellipse.getCenter().x, 100.0);
    EXPECT_EQ(ellipse.getCenter().y, 200.0);
}

TEST_F(SvgElementTest, SvgEllipseNegativeRadiiClamped) {
    SvgEllipse ellipse1(Point{0, 0}, -30, 20);
    EXPECT_EQ(ellipse1.getRx(), 0.0);
    SvgEllipse ellipse2(Point{0, 0}, 30, -20);
    EXPECT_EQ(ellipse2.getRy(), 0.0);
}

TEST_F(SvgElementTest, SvgPolygonSetPoints) {
    SvgPolygon polygon;
    polygon.setPoints({{0, 0}, {10, 10}, {20, 0}});
    EXPECT_EQ(polygon.getPoints().size(), 3);
}

TEST_F(SvgElementTest, SvgPolylineToSvgStringHasFillNone) {
    SvgPolyline polyline({{0, 0}, {10, 10}, {20, 0}});
    std::string svg = polyline.toSvgString();
    EXPECT_TRUE(svg.find("fill=\"none\"") != std::string::npos);
}

TEST_F(SvgElementTest, SvgStarWithTwoPoints) {
    SvgStar star(Point{100, 100}, 50, 25, 2, 0);
    EXPECT_EQ(star.getPoints().size(), 4);
}

TEST_F(SvgElementTest, SvgElementTransform) {
    SvgRectangle rect;
    Transform t;
    t.transform_str = "rotate(45)";
    rect.setTransform(t);
    EXPECT_EQ(rect.getTransform().transform_str, "rotate(45)");
}

TEST_F(SvgElementTest, GetCommonAttributesStringWithTransform) {
    SvgRectangle rect;
    Transform t;
    t.transform_str = "rotate(45)";
    rect.setTransform(t);
    std::string attrs = rect.getCommonAttributesString();
    EXPECT_TRUE(attrs.find("transform=") != std::string::npos);
}

TEST_F(SvgElementTest, SvgLineWithParameters) {
    SvgLine line(Point{10, 20}, Point{30, 40});
    EXPECT_EQ(line.getP1().x, 10.0);
    EXPECT_EQ(line.getP1().y, 20.0);
    EXPECT_EQ(line.getP2().x, 30.0);
    EXPECT_EQ(line.getP2().y, 40.0);
}

TEST_F(SvgElementTest, SvgCircleWithParameters) {
    SvgCircle circle(Point{100, 100}, 50);
    EXPECT_EQ(circle.getCenter().x, 100.0);
    EXPECT_EQ(circle.getCenter().y, 100.0);
    EXPECT_EQ(circle.getRadius(), 50.0);
}

TEST_F(SvgElementTest, SvgEllipseWithParameters) {
    SvgEllipse ellipse(Point{100, 100}, 50, 30);
    EXPECT_EQ(ellipse.getCenter().x, 100.0);
    EXPECT_EQ(ellipse.getCenter().y, 100.0);
    EXPECT_EQ(ellipse.getRx(), 50.0);
    EXPECT_EQ(ellipse.getRy(), 30.0);
}

TEST_F(SvgElementTest, SvgPolygonWithPoints) {
    std::vector<Point> pts = {{0, 0}, {10, 10}, {20, 0}, {10, -10}};
    SvgPolygon polygon(pts);
    EXPECT_EQ(polygon.getPoints().size(), 4);
}

TEST_F(SvgElementTest, SvgPolylineWithPoints) {
    std::vector<Point> pts = {{0, 0}, {10, 10}, {20, 0}};
    SvgPolyline polyline(pts);
    EXPECT_EQ(polyline.getPoints().size(), 3);
}

TEST_F(SvgElementTest, SvgPentagonWithCenterAndRadius) {
    SvgPentagon pentagon(Point{100, 100}, 50);
    EXPECT_EQ(pentagon.getPoints().size(), 5);
}

TEST_F(SvgElementTest, SvgHexagonWithCenterAndRadius) {
    SvgHexagon hexagon(Point{100, 100}, 50);
    EXPECT_EQ(hexagon.getPoints().size(), 6);
}

TEST_F(SvgElementTest, SvgStarWithParameters) {
    SvgStar star(Point{100, 100}, 50, 25, 5, 0);
    EXPECT_EQ(star.getPoints().size(), 10);
}

TEST_F(SvgElementTest, SvgLineToSvgString) {
    SvgLine line(Point{10, 20}, Point{30, 40});
    std::string svg = line.toSvgString();
    EXPECT_TRUE(svg.find("<line") != std::string::npos);
    EXPECT_TRUE(svg.find("x1=\"10\"") != std::string::npos);
}

TEST_F(SvgElementTest, SvgCircleToSvgString) {
    SvgCircle circle(Point{100, 100}, 50);
    std::string svg = circle.toSvgString();
    EXPECT_TRUE(svg.find("<circle") != std::string::npos);
    EXPECT_TRUE(svg.find("cx=\"100\"") != std::string::npos);
    EXPECT_TRUE(svg.find("r=\"50\"") != std::string::npos);
}
