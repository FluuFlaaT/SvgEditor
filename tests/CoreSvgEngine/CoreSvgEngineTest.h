#pragma once
#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgShapes.h"
#include "../../src/CoreSvgEngine/CoreSvgEngine.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

class CoreSvgEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = new CoreSvgEngine();
    }

    void TearDown() override {
        delete engine;
    }

    // 辅助方法：创建临时SVG文件
    std::string createTempSvgFile(const std::string& content) {
        std::string tempFilePath = "temp_test_file.svg";
        std::ofstream outFile(tempFilePath);
        outFile << content;
        outFile.close();
        return tempFilePath;
    }

    // 辅助方法：删除临时文件
    void deleteTempFile(const std::string& filePath) {
        std::remove(filePath.c_str());
    }

    CoreSvgEngine* engine;
};
