#include <gtest/gtest.h>
#include <Core/ShapeType.h>

class ShapeTypeTest : public ::testing::Test {
protected:
};

TEST_F(ShapeTypeTest, AllShapeTypesDefined) {
    EXPECT_TRUE(ShapeType::None != ShapeType::Line);
    EXPECT_TRUE(ShapeType::Line != ShapeType::Rectangle);
    EXPECT_TRUE(ShapeType::Rectangle != ShapeType::Ellipse);
    EXPECT_TRUE(ShapeType::Ellipse != ShapeType::Pentagon);
    EXPECT_TRUE(ShapeType::Pentagon != ShapeType::Hexagon);
    EXPECT_TRUE(ShapeType::Hexagon != ShapeType::Star);
    EXPECT_TRUE(ShapeType::Star != ShapeType::Freehand);
    EXPECT_TRUE(ShapeType::Freehand != ShapeType::Text);
}

TEST_F(ShapeTypeTest, ShapeTypeCount) {
    int enumCount = 0;
    ShapeType types[] = {
        ShapeType::None,
        ShapeType::Line,
        ShapeType::Rectangle,
        ShapeType::Ellipse,
        ShapeType::Pentagon,
        ShapeType::Hexagon,
        ShapeType::Star,
        ShapeType::Freehand,
        ShapeType::Text
    };
    enumCount = sizeof(types) / sizeof(types[0]);
    EXPECT_EQ(enumCount, 9);
}

TEST_F(ShapeTypeTest, ShapeTypeCanBeUsedInSwitch) {
    ShapeType type = ShapeType::Rectangle;
    int result = 0;
    switch (type) {
        case ShapeType::None: result = 0; break;
        case ShapeType::Line: result = 1; break;
        case ShapeType::Rectangle: result = 2; break;
        case ShapeType::Ellipse: result = 3; break;
        case ShapeType::Pentagon: result = 4; break;
        case ShapeType::Hexagon: result = 5; break;
        case ShapeType::Star: result = 6; break;
        case ShapeType::Freehand: result = 7; break;
        case ShapeType::Text: result = 8; break;
    }
    EXPECT_EQ(result, 2);
}
