#pragma once
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