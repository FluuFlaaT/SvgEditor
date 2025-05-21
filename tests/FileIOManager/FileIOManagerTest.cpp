#include <gtest/gtest.h>
#include "../../src/FileIOManager/FileIOManager.h"
#include "../../src/CoreSvgEngine/CoreSvgEngine.h"
#include "../../src/CoreSvgEngine/SvgShapes.h"
#include "../../src/CoreSvgEngine/SvgDocument.h" // Add SvgDocument.h include
#include <string>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <memory> // Add for std::unique_ptr

class FileIOManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // engine = new CoreSvgEngine(); // CoreSvgEngine is no longer needed by FileIOManager
        fileManager = new FileIOManager();
        
        // 创建临时测试目录
        tempDir = new QTemporaryDir();
        ASSERT_TRUE(tempDir->isValid());
        testDirPath = tempDir->path();
        
        // 准备测试用SVG文件
        validSvgContent = "<svg width=\"400\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">"
                         "<rect x=\"10\" y=\"10\" width=\"100\" height=\"80\" fill=\"blue\" />"
                         "</svg>";
        
        // 创建有效的测试SVG文件
        validSvgFilePath = testDirPath + "/valid_test.svg";
        QFile validFile(validSvgFilePath);
        ASSERT_TRUE(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
        validFile.write(validSvgContent.toUtf8());
        validFile.close();
        
        // 创建无效的测试SVG文件
        invalidSvgFilePath = testDirPath + "/invalid_test.svg";
        QFile invalidFile(invalidSvgFilePath);
        ASSERT_TRUE(invalidFile.open(QIODevice::WriteOnly | QIODevice::Text));
        invalidFile.write("<invalid>svg content</invalid>");
        invalidFile.close();
    }

    void TearDown() override {
        delete fileManager;
        // delete engine; // CoreSvgEngine is no longer needed
        delete tempDir; // 这将自动清理临时目录及其内容
    }

    // CoreSvgEngine* engine; // CoreSvgEngine is no longer needed
    FileIOManager* fileManager;
    QTemporaryDir* tempDir;
    QString testDirPath;
    QString validSvgFilePath;
    QString invalidSvgFilePath;
    QString validSvgContent;
};

// 测试打开有效的SVG文件
TEST_F(FileIOManagerTest, OpenValidSvgFile) {
    std::unique_ptr<SvgDocument> doc; // Use std::unique_ptr
    bool result = fileManager->openSvgFile(validSvgFilePath, doc);
    
    EXPECT_TRUE(result) << "Should successfully open valid SVG file";
    EXPECT_NE(nullptr, doc.get()) << "Document should not be null"; // Use .get()
    
    if (doc) {
        EXPECT_EQ(400, doc->getWidth());
        EXPECT_EQ(300, doc->getHeight());
        EXPECT_EQ(1, doc->getElements().size()); // One rectangle element
        
        // No need to delete doc, unique_ptr handles it
    }
}

// 测试打开无效的SVG文件
TEST_F(FileIOManagerTest, OpenInvalidSvgFile) {
    std::unique_ptr<SvgDocument> doc; // Use std::unique_ptr
    bool result = fileManager->openSvgFile(invalidSvgFilePath, doc);
    
    EXPECT_FALSE(result) << "Should not be able to open invalid SVG file";
    EXPECT_EQ(nullptr, doc.get()) << "Invalid file should return null document"; // Use .get()
    EXPECT_FALSE(fileManager->lastError().isEmpty()) << "Error message should be set";
}

// 测试打开不存在的文件
TEST_F(FileIOManagerTest, OpenNonExistentFile) {
    std::unique_ptr<SvgDocument> doc; // Use std::unique_ptr
    QString nonExistentPath = testDirPath + "/non_existent.svg";
    bool result = fileManager->openSvgFile(nonExistentPath, doc);
    
    EXPECT_FALSE(result) << "Should not be able to open non-existent file";
    EXPECT_EQ(nullptr, doc.get()) << "Non-existent file should return null document"; // Use .get()
    EXPECT_FALSE(fileManager->lastError().isEmpty()) << "Error message should be set";
}

// 测试保存SVG文件
TEST_F(FileIOManagerTest, SaveSvgFile) {
    // 创建一个文档并添加一些元素
    auto doc = std::make_unique<SvgDocument>(500, 400); // Use std::make_unique
    
    // 保存文档到新文件
    QString savePath = testDirPath + "/saved_test.svg";
    bool saveResult = fileManager->saveSvgFile(savePath, doc.get()); // Use .get()
    
    EXPECT_TRUE(saveResult) << "Should be able to save SVG file successfully";
    EXPECT_TRUE(QFile::exists(savePath)) << "Saved file should exist";
    
    // 尝试重新加载保存的文件并验证内容
    std::unique_ptr<SvgDocument> loadedDoc; // Use std::unique_ptr
    bool loadResult = fileManager->openSvgFile(savePath, loadedDoc);
    
    EXPECT_TRUE(loadResult) << "Should be able to reload saved file";
    EXPECT_NE(nullptr, loadedDoc.get()) << "Loaded document should not be null"; // Use .get()
    
    if (loadedDoc) {
        EXPECT_EQ(500, loadedDoc->getWidth());
        EXPECT_EQ(400, loadedDoc->getHeight());
        // No need to delete loadedDoc
    }
    
    // No need to delete doc
}

// 测试保存空文档
TEST_F(FileIOManagerTest, SaveNullDocument) {
    QString savePath = testDirPath + "/null_doc_test.svg";
    bool result = fileManager->saveSvgFile(savePath, nullptr);
    
    EXPECT_FALSE(result) << "Saving null document should fail";
    EXPECT_FALSE(fileManager->lastError().isEmpty()) << "Error message should be set";
}

// 测试保存到无效路径
TEST_F(FileIOManagerTest, SaveToInvalidPath) {
    auto doc = std::make_unique<SvgDocument>(); // Use std::make_unique
    
    // 尝试保存到一个不存在的目录
    QString invalidPath = "/non_existent_dir/test.svg";
    bool result = fileManager->saveSvgFile(invalidPath, doc.get()); // Use .get()
    
    EXPECT_FALSE(result) << "Saving to invalid path should fail";
    EXPECT_FALSE(fileManager->lastError().isEmpty()) << "Error message should be set";
}

// 测试lastError方法
TEST_F(FileIOManagerTest, LastErrorTest) {
    // 首先确保没有错误
    EXPECT_TRUE(fileManager->lastError().isEmpty()) << "Initial state should have no error";
    
    // 尝试打开不存在的文件，应产生错误
    std::unique_ptr<SvgDocument> doc; // Use std::unique_ptr
    fileManager->openSvgFile("/non_existent.svg", doc);
    
    EXPECT_FALSE(fileManager->lastError().isEmpty()) << "Should have error message after failed operation";
    
    // 尝试成功操作后，错误应清除
    QString savedErrorMessage = fileManager->lastError(); // Save error message to verify it actually changed
    bool result = fileManager->openSvgFile(validSvgFilePath, doc);
    
    EXPECT_TRUE(result);
    EXPECT_NE(savedErrorMessage, fileManager->lastError()) << "New operation should update error message";
    // After a successful open, lastError might be cleared or might hold info about the opened file.
    // For consistency with previous logic, let's assume it's cleared on success.
    // If openSvgFile sets a success message, this test might need adjustment.
    // Based on current FileIOManager::openSvgFile, it only sets m_lastError on failure.
    EXPECT_TRUE(fileManager->lastError().isEmpty()) << "Error should be cleared after successful operation"; 
    
    // No need to delete doc
}

// 测试保存和加载后SVG元素的顺序是否保持不变
TEST_F(FileIOManagerTest, PreserveElementOrderWhenSaveAndLoad) {
    // 创建一个有多个元素的文档，顺序为：矩形、圆形、线条
    auto originalDoc = std::make_unique<SvgDocument>(600, 400); // Use std::make_unique
    
    // 添加矩形
    auto rect = std::make_unique<SvgRectangle>(Point{50, 50}, 100, 80); // Use std::make_unique
    rect->setFillColor({255, 0, 0, 255});  // 红色
    rect->setID("rect1");
    originalDoc->addElement(std::move(rect)); // Use std::move
    
    // 添加圆形
    auto circle = std::make_unique<SvgCircle>(Point{200, 150}, 60); // Use std::make_unique
    circle->setFillColor({0, 255, 0, 255});  // 绿色
    circle->setID("circle1");
    originalDoc->addElement(std::move(circle)); // Use std::move
    
    // 添加线条
    auto line = std::make_unique<SvgLine>(Point{100, 200}, Point{300, 300}); // Use std::make_unique
    line->setStrokeColor({0, 0, 255, 255});  // 蓝色
    line->setStrokeWidth(3.0);
    line->setID("line1");
    originalDoc->addElement(std::move(line)); // Use std::move
    
    // 保存文档到文件
    QString orderTestPath = testDirPath + "/element_order_test.svg";
    bool saveResult = fileManager->saveSvgFile(orderTestPath, originalDoc.get()); // Use .get()
    ASSERT_TRUE(saveResult) << "Failed to save SVG document";
    
    // 重新加载文档
    std::unique_ptr<SvgDocument> loadedDoc; // Use std::unique_ptr
    bool loadResult = fileManager->openSvgFile(orderTestPath, loadedDoc);
    ASSERT_TRUE(loadResult) << "Failed to load SVG document";
    ASSERT_NE(nullptr, loadedDoc.get()) << "Loaded document is null"; // Use .get()
    
    // 验证元素数量
    ASSERT_EQ(originalDoc->getElements().size(), loadedDoc->getElements().size()) 
        << "Element count mismatch after loading";
    
    // 验证元素顺序与类型
    const auto& originalElems = originalDoc->getElements(); // Explicitly use const auto&
    const auto& loadedElems = loadedDoc->getElements();   // Explicitly use const auto&
    
    for (size_t i = 0; i < originalElems.size(); i++) {
        EXPECT_EQ(originalElems[i]->getType(), loadedElems[i]->getType())
            << "Element type at position " << i << " doesn't match";
        EXPECT_EQ(originalElems[i]->getID(), loadedElems[i]->getID())
            << "Element ID at position " << i << " doesn't match";
    }
    
    // 验证具体元素属性（可选，但更彻底）
    if (loadedElems.size() >= 3) {
        // 检查矩形
        SvgRectangle* loadedRect = dynamic_cast<SvgRectangle*>(loadedElems[0].get()); // Use .get()
        ASSERT_NE(nullptr, loadedRect) << "First element is not a rectangle";
        EXPECT_EQ("rect1", loadedRect->getID());
        
        // 检查圆形
        SvgCircle* loadedCircle = dynamic_cast<SvgCircle*>(loadedElems[1].get()); // Use .get()
        ASSERT_NE(nullptr, loadedCircle) << "Second element is not a circle";
        EXPECT_EQ("circle1", loadedCircle->getID());
        
        // 检查线条
        SvgLine* loadedLine = dynamic_cast<SvgLine*>(loadedElems[2].get()); // Use .get()
        ASSERT_NE(nullptr, loadedLine) << "Third element is not a line";
        EXPECT_EQ("line1", loadedLine->getID());
    }
    
    // 清理
    // No need to delete originalDoc or loadedDoc
}
