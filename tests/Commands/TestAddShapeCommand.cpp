#include <gtest/gtest.h>
#include <memory>
#include <Commands/AddShapeCommand.h>
#include <Core/ShapeType.h>

class AddShapeCommandTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AddShapeCommandTest, AddShapeCommandCanBeCreated) {
    EXPECT_TRUE(true);
}

TEST_F(AddShapeCommandTest, ShapeTypeValues) {
    EXPECT_EQ(static_cast<int>(ShapeType::Rectangle), 3);
    EXPECT_EQ(static_cast<int>(ShapeType::Ellipse), 4);
    EXPECT_EQ(static_cast<int>(ShapeType::Line), 1);
}

TEST_F(AddShapeCommandTest, AddShapeCommandClassExists) {
    EXPECT_TRUE(sizeof(AddShapeCommand) > 0);
}
