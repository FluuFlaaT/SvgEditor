#pragma once
#include <gtest/gtest.h>
#include "../../src/CoreSvgEngine/SvgParser.h"
#include "../../src/CoreSvgEngine/SvgDocument.h"
#include <memory>

class SvgParserTest : public ::testing::Test {
protected:
    std::unique_ptr<SvgDocument> doc;

    void SetUp() override {
        doc = std::make_unique<SvgDocument>();
    }

    // void TearDown() override {} // Not needed for unique_ptr
};
