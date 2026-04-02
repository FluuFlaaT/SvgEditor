#include <gtest/gtest.h>
#include <memory>
#include <CoreSvgEngine/SvgDocument.h>
#include <CoreSvgEngine/SvgShapes.h>

class SvgDocumentTest : public ::testing::Test {
protected:
    void SetUp() override {
        doc = std::make_unique<SvgDocument>(800, 600);
    }

    std::unique_ptr<SvgDocument> doc;
};

TEST_F(SvgDocumentTest, DefaultDimensions) {
    EXPECT_EQ(doc->getWidth(), 800.0);
    EXPECT_EQ(doc->getHeight(), 600.0);
}

TEST_F(SvgDocumentTest, InitialElementCountIsZero) {
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(SvgDocumentTest, AddShapeIncreasesCount) {
    auto rect = std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50);
    rect->setID("rect1");
    doc->addElement(std::move(rect));
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, AddMultipleShapesIncreasesCount) {
    doc->addElement(std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50));
    doc->addElement(std::make_unique<SvgEllipse>(Point{50, 50}, 30, 20));
    doc->addElement(std::make_unique<SvgLine>(Point{0, 0}, Point{100, 100}));
    EXPECT_EQ(doc->getElements().size(), 3);
}

TEST_F(SvgDocumentTest, RemoveElementById) {
    auto rect = std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50);
    rect->setID("rect1");
    doc->addElement(std::move(rect));
    EXPECT_EQ(doc->getElements().size(), 1);
    
    bool removed = doc->removeElementById("rect1");
    EXPECT_TRUE(removed);
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(SvgDocumentTest, RemoveNonExistentElementReturnsFalse) {
    bool removed = doc->removeElementById("nonexistent");
    EXPECT_FALSE(removed);
}

TEST_F(SvgDocumentTest, ClearElementsRemovesAll) {
    doc->addElement(std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50));
    doc->addElement(std::make_unique<SvgEllipse>(Point{50, 50}, 30, 20));
    EXPECT_EQ(doc->getElements().size(), 2);
    
    doc->clearElements();
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(SvgDocumentTest, SetDimensions) {
    doc->setWidth(1024);
    doc->setHeight(768);
    EXPECT_EQ(doc->getWidth(), 1024.0);
    EXPECT_EQ(doc->getHeight(), 768.0);
}

TEST_F(SvgDocumentTest, SetBackgroundColor) {
    Color bgColor = {255, 0, 0, 255};
    doc->setBackgroundColor(bgColor);
    EXPECT_EQ(doc->getBackgroundColor().r, 255);
    EXPECT_EQ(doc->getBackgroundColor().g, 0);
    EXPECT_EQ(doc->getBackgroundColor().b, 0);
}

TEST_F(SvgDocumentTest, GenerateSvgContentContainsShapes) {
    auto rect = std::make_unique<SvgRectangle>(Point{10, 20}, 100, 50);
    rect->setID("rect1");
    doc->addElement(std::move(rect));
    
    std::string svg = doc->generateSvgContent();
    EXPECT_TRUE(svg.find("<rect") != std::string::npos);
    EXPECT_TRUE(svg.find("rect1") != std::string::npos);
}

TEST_F(SvgDocumentTest, GenerateSvgContentContainsSvgTag) {
    std::string svg = doc->generateSvgContent();
    EXPECT_TRUE(svg.find("<svg") != std::string::npos);
    EXPECT_TRUE(svg.find("width=\"800\"") != std::string::npos);
    EXPECT_TRUE(svg.find("height=\"600\"") != std::string::npos);
}

TEST_F(SvgDocumentTest, RemoveElementByPointer) {
    auto rect = std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50);
    rect->setID("rect1");
    SvgRectangle* rectPtr = rect.get();
    doc->addElement(std::move(rect));
    EXPECT_EQ(doc->getElements().size(), 1);
    
    bool removed = doc->removeElement(rectPtr);
    EXPECT_TRUE(removed);
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(SvgDocumentTest, RemoveElementByNullPointerReturnsFalse) {
    bool removed = doc->removeElement(nullptr);
    EXPECT_FALSE(removed);
}

TEST_F(SvgDocumentTest, SetWidthWithZeroClampedToOne) {
    doc->setWidth(0);
    EXPECT_EQ(doc->getWidth(), 1.0);
}

TEST_F(SvgDocumentTest, SetWidthWithNegativeClampedToOne) {
    doc->setWidth(-100);
    EXPECT_EQ(doc->getWidth(), 1.0);
}

TEST_F(SvgDocumentTest, SetHeightWithZeroClampedToOne) {
    doc->setHeight(0);
    EXPECT_EQ(doc->getHeight(), 1.0);
}

TEST_F(SvgDocumentTest, SetHeightWithNegativeClampedToOne) {
    doc->setHeight(-100);
    EXPECT_EQ(doc->getHeight(), 1.0);
}

TEST_F(SvgDocumentTest, ParseInvalidSvgReturnsFalse) {
    std::string invalidSvg = "not a valid svg";
    bool result = doc->parseSvgContent(invalidSvg);
    EXPECT_FALSE(result);
}

TEST_F(SvgDocumentTest, ParseSvgWithoutSvgTagReturnsFalse) {
    std::string noSvgTag = "<rect x='0' y='0' width='100' height='100'/>";
    bool result = doc->parseSvgContent(noSvgTag);
    EXPECT_FALSE(result);
}

TEST_F(SvgDocumentTest, ParseSvgWithRectangle) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <rect id="testRect" x="10" y="20" width="100" height="50" fill="red"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithCircle) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <circle id="testCircle" cx="100" cy="100" r="50" fill="blue"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithEllipse) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <ellipse id="testEllipse" cx="100" cy="100" rx="50" ry="30" fill="green"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithLine) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <line id="testLine" x1="0" y1="0" x2="100" y2="100" stroke="black"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithPolygon) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <polygon id="testPolygon" points="100,0 200,100 100,200 0,100" fill="yellow"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithPolyline) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <polyline id="testPolyline" points="0,100 50,0 100,100" stroke="black" fill="none"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWith100PercentBackground) {
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <rect width="100%" height="100%" fill="gray"/>
  <rect id="testRect" x="10" y="20" width="100" height="50"/>
</svg>)";
    bool result = doc->parseSvgContent(svg);
    EXPECT_TRUE(result);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, ParseSvgWithBackgroundClearsExisting) {
    doc->addElement(std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50));
    EXPECT_EQ(doc->getElements().size(), 1);
    
    std::string svg = R"(<?xml version="1.0"?>
<svg width="800" height="600" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="20" width="100" height="50"/>
</svg>)";
    doc->parseSvgContent(svg);
    EXPECT_EQ(doc->getElements().size(), 1);
}

TEST_F(SvgDocumentTest, GenerateSvgContentWithBackground) {
    doc->setBackgroundColor({128, 128, 128, 255});
    std::string svg = doc->generateSvgContent();
    EXPECT_TRUE(svg.find("fill=") != std::string::npos);
}

TEST_F(SvgDocumentTest, GenerateSvgContentWithWhiteBackgroundNoFill) {
    doc->setBackgroundColor({255, 255, 255, 255});
    std::string svg = doc->generateSvgContent();
    EXPECT_TRUE(svg.find("<rect") == std::string::npos);
}
