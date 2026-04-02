#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <iostream>
#include <CoreSvgEngine/SvgDocument.h>
#include <CoreSvgEngine/SvgShapes.h>
#include <CoreSvgEngine/CoreSvgEngine.h>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<CoreSvgEngine>();
        engine->createNewDocument(800, 600);
    }

    void TearDown() override {
        engine.reset();
    }

    std::string createTempSvgFile() {
        std::string tempPath = "test_temp_" + std::to_string(reinterpret_cast<size_t>(this)) + ".svg";
        return tempPath;
    }

    void cleanupTempFile(const std::string& path) {
        std::remove(path.c_str());
    }

    std::unique_ptr<CoreSvgEngine> engine;
};

TEST_F(IntegrationTest, CreateSaveLoadVerifyShapes) {
    auto doc = engine->getCurrentDocument();
    
    auto rect = std::make_unique<SvgRectangle>(Point{10, 20}, 100, 50);
    rect->setID("rect1");
    doc->addElement(std::move(rect));
    
    auto ellipse = std::make_unique<SvgEllipse>(Point{100, 100}, 30, 20);
    ellipse->setID("ellipse1");
    doc->addElement(std::move(ellipse));
    
    EXPECT_EQ(doc->getElements().size(), 2);
    
    std::string svgContent = doc->generateSvgContent();
    EXPECT_TRUE(svgContent.find("<rect") != std::string::npos);
    EXPECT_TRUE(svgContent.find("<ellipse") != std::string::npos);
}

TEST_F(IntegrationTest, CreateNewDocumentWithBackground) {
    auto doc = engine->getCurrentDocument();
    
    Color bgColor = {255, 200, 200, 255};
    doc->setBackgroundColor(bgColor);
    
    EXPECT_EQ(doc->getBackgroundColor().r, 255);
    EXPECT_EQ(doc->getBackgroundColor().g, 200);
    EXPECT_EQ(doc->getBackgroundColor().b, 200);
}

TEST_F(IntegrationTest, DocumentDimensions) {
    auto doc = engine->getCurrentDocument();
    
    EXPECT_EQ(doc->getWidth(), 800.0);
    EXPECT_EQ(doc->getHeight(), 600.0);
    
    doc->setWidth(1024);
    doc->setHeight(768);
    
    EXPECT_EQ(doc->getWidth(), 1024.0);
    EXPECT_EQ(doc->getHeight(), 768.0);
}

TEST_F(IntegrationTest, AddMultipleShapesOfDifferentTypes) {
    auto doc = engine->getCurrentDocument();
    
    doc->addElement(std::make_unique<SvgLine>(Point{0, 0}, Point{100, 100}));
    doc->addElement(std::make_unique<SvgRectangle>(Point{0, 0}, 50, 50));
    doc->addElement(std::make_unique<SvgCircle>(Point{50, 50}, 25));
    doc->addElement(std::make_unique<SvgEllipse>(Point{100, 100}, 30, 20));
    
    EXPECT_EQ(doc->getElements().size(), 4);
}

TEST_F(IntegrationTest, RemoveShapeById) {
    auto doc = engine->getCurrentDocument();
    
    auto rect = std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50);
    rect->setID("test_rect");
    doc->addElement(std::move(rect));
    
    EXPECT_EQ(doc->getElements().size(), 1);
    
    bool removed = doc->removeElementById("test_rect");
    EXPECT_TRUE(removed);
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(IntegrationTest, RemoveNonExistentShape) {
    auto doc = engine->getCurrentDocument();
    
    bool removed = doc->removeElementById("nonexistent_id");
    EXPECT_FALSE(removed);
}

TEST_F(IntegrationTest, ClearAllShapes) {
    auto doc = engine->getCurrentDocument();
    
    doc->addElement(std::make_unique<SvgRectangle>(Point{0, 0}, 100, 50));
    doc->addElement(std::make_unique<SvgEllipse>(Point{50, 50}, 30, 20));
    doc->addElement(std::make_unique<SvgLine>(Point{0, 0}, Point{100, 100}));
    
    EXPECT_EQ(doc->getElements().size(), 3);
    
    doc->clearElements();
    
    EXPECT_EQ(doc->getElements().size(), 0);
}

TEST_F(IntegrationTest, GenerateSvgContentStructure) {
    auto doc = engine->getCurrentDocument();
    doc->setWidth(800);
    doc->setHeight(600);
    
    std::string svgContent = doc->generateSvgContent();
    
    EXPECT_TRUE(svgContent.find("<svg") != std::string::npos);
    EXPECT_TRUE(svgContent.find("width=\"800\"") != std::string::npos);
    EXPECT_TRUE(svgContent.find("height=\"600\"") != std::string::npos);
    EXPECT_TRUE(svgContent.find("xmlns=\"http://www.w3.org/2000/svg\"") != std::string::npos);
    EXPECT_TRUE(svgContent.find("</svg>") != std::string::npos);
}

TEST_F(IntegrationTest, SvgLineToSvgString) {
    SvgLine line(Point{10, 20}, Point{100, 200});
    line.setID("line1");
    line.setStrokeColor({0, 0, 0, 255});
    line.setStrokeWidth(2.0);
    
    std::string svgStr = line.toSvgString();
    
    EXPECT_TRUE(svgStr.find("<line") != std::string::npos);
    EXPECT_TRUE(svgStr.find("x1=\"10\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("y1=\"20\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("x2=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("y2=\"200\"") != std::string::npos);
}

TEST_F(IntegrationTest, SvgRectangleToSvgString) {
    SvgRectangle rect(Point{10, 20}, 100, 50);
    rect.setID("rect1");
    rect.setFillColor({255, 0, 0, 255});
    rect.setStrokeColor({0, 0, 0, 255});
    rect.setStrokeWidth(1.0);
    
    std::string svgStr = rect.toSvgString();
    
    EXPECT_TRUE(svgStr.find("<rect") != std::string::npos);
    EXPECT_TRUE(svgStr.find("x=\"10\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("y=\"20\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("width=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("height=\"50\"") != std::string::npos);
}

TEST_F(IntegrationTest, SvgEllipseToSvgString) {
    SvgEllipse ellipse(Point{100, 100}, 50, 30);
    ellipse.setID("ellipse1");
    
    std::string svgStr = ellipse.toSvgString();
    
    EXPECT_TRUE(svgStr.find("<ellipse") != std::string::npos);
    EXPECT_TRUE(svgStr.find("cx=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("cy=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("rx=\"50\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("ry=\"30\"") != std::string::npos);
}

TEST_F(IntegrationTest, SvgCircleToSvgString) {
    SvgCircle circle(Point{100, 100}, 50);
    circle.setID("circle1");
    
    std::string svgStr = circle.toSvgString();
    
    EXPECT_TRUE(svgStr.find("<circle") != std::string::npos);
    EXPECT_TRUE(svgStr.find("cx=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("cy=\"100\"") != std::string::npos);
    EXPECT_TRUE(svgStr.find("r=\"50\"") != std::string::npos);
}
