#include "CoreSvgEngineTest.h"

// 测试引擎初始化
TEST_F(CoreSvgEngineTest, Initialization) {
    ASSERT_NE(nullptr, engine);
    
    SvgDocument* doc = engine->getCurrentDocument();
    ASSERT_NE(nullptr, doc);
    
    // 检查默认文档属性
    EXPECT_EQ(600, doc->getWidth());
    EXPECT_EQ(400, doc->getHeight());
    
    // 检查背景色是否为默认白色
    Color bg = doc->getBackgroundColor();
    EXPECT_EQ(255, bg.r);
    EXPECT_EQ(255, bg.g);
    EXPECT_EQ(255, bg.b);
    EXPECT_EQ(255, bg.alpha);
}

// 测试创建新文档
TEST_F(CoreSvgEngineTest, CreateNewDocument) {
    // 创建自定义尺寸和颜色的文档
    engine->createNewDocument(800, 600, {200, 200, 200, 255});
    
    SvgDocument* doc = engine->getCurrentDocument();
    ASSERT_NE(nullptr, doc);
    
    // 验证尺寸和颜色
    EXPECT_EQ(800, doc->getWidth());
    EXPECT_EQ(600, doc->getHeight());
    
    Color bg = doc->getBackgroundColor();
    EXPECT_EQ(200, bg.r);
    EXPECT_EQ(200, bg.g);
    EXPECT_EQ(200, bg.b);
    EXPECT_EQ(255, bg.alpha);
    
    // 确认文档是空的
    EXPECT_EQ(0, doc->getElements().size());
}

// 测试保存和加载SVG文件
TEST_F(CoreSvgEngineTest, SaveAndLoadSvgFile) {
    // 创建带有一些元素的文档
    engine->createNewDocument(400, 300, {255, 255, 255, 255});
    SvgDocument* doc = engine->getCurrentDocument();
    
    // 添加一个矩形
    SvgRectangle* rect = new SvgRectangle({50, 50}, 200, 100);
    rect->setID("test-rect");
    rect->setFillColor({0, 0, 255, 255}); // 蓝色填充
    doc->addElement(rect);
    
    // 添加一个圆形
    SvgCircle* circle = new SvgCircle({150, 150}, 80);
    circle->setID("test-circle");
    circle->setFillColor({255, 0, 0, 255}); // 红色填充
    doc->addElement(circle);
    
    // 保存到临时文件
    std::string tempFilePath = "temp_svg_test.svg";
    EXPECT_TRUE(engine->saveSvgFile(tempFilePath));
    
    // 创建新的引擎实例并加载文件
    CoreSvgEngine loadEngine;
    EXPECT_TRUE(loadEngine.loadSvgFile(tempFilePath));
    
    // 验证加载的文档
    SvgDocument* loadedDoc = loadEngine.getCurrentDocument();
    ASSERT_NE(nullptr, loadedDoc);
    
    // 检查文档属性
    EXPECT_EQ(400, loadedDoc->getWidth());
    EXPECT_EQ(300, loadedDoc->getHeight());
    
    // 检查元素数量
    EXPECT_EQ(2, loadedDoc->getElements().size());
    
    // 检查第一个元素 - 矩形
    SvgElement* elem1 = loadedDoc->getElements()[0];
    ASSERT_EQ(SvgElementType::Rectangle, elem1->getType());
    
    SvgRectangle* loadedRect = dynamic_cast<SvgRectangle*>(elem1);
    ASSERT_NE(nullptr, loadedRect);
    EXPECT_EQ("test-rect", loadedRect->getID());
    EXPECT_EQ(50, loadedRect->getTopLeft().x);
    EXPECT_EQ(50, loadedRect->getTopLeft().y);
    EXPECT_EQ(200, loadedRect->getWidth());
    EXPECT_EQ(100, loadedRect->getHeight());
    
    // 检查第二个元素 - 圆形
    SvgElement* elem2 = loadedDoc->getElements()[1];
    ASSERT_EQ(SvgElementType::Circle, elem2->getType());
    
    SvgCircle* loadedCircle = dynamic_cast<SvgCircle*>(elem2);
    ASSERT_NE(nullptr, loadedCircle);
    EXPECT_EQ("test-circle", loadedCircle->getID());
    EXPECT_EQ(150, loadedCircle->getCenter().x);
    EXPECT_EQ(150, loadedCircle->getCenter().y);
    EXPECT_EQ(80, loadedCircle->getRadius());
    
    // 清理临时文件
    std::remove(tempFilePath.c_str());
}

// 测试加载错误处理 - 尝试加载不存在的文件
TEST_F(CoreSvgEngineTest, LoadNonExistentFile) {
    EXPECT_FALSE(engine->loadSvgFile("non_existent_file.svg"));
}

// 测试加载错误处理 - 尝试加载无效的SVG内容
TEST_F(CoreSvgEngineTest, LoadInvalidSvgContent) {
    // 创建一个包含无效SVG内容的临时文件
    std::string tempFilePath = "invalid_svg_test.svg";
    std::ofstream outFile(tempFilePath);
    outFile << "<not-valid-svg>";
    outFile.close();
    
    // 尝试加载该文件
    EXPECT_FALSE(engine->loadSvgFile(tempFilePath));
    
    // 清理临时文件
    std::remove(tempFilePath.c_str());
}
