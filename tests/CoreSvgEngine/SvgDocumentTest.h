#pragma once
#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgDocument.h"
#include "../../src/CoreSvgEngine/SvgShapes.h"
#include "../../src/CoreSvgEngine/SvgText.h"
#include <iostream>
#include <sstream>
#include <string>
#include <memory> // Required for std::unique_ptr

class SvgDocumentTest : public ::testing::Test {
protected:
    void SetUp() override {
        doc = std::make_unique<SvgDocument>(800, 600);
    }

    std::unique_ptr<SvgDocument> doc;
};