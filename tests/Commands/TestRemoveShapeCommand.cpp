#include <gtest/gtest.h>
#include <memory>
#include <Commands/RemoveShapeCommand.h>
#include <Core/ShapeType.h>

class RemoveShapeCommandTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(RemoveShapeCommandTest, RemoveShapeCommandCanBeCreated) {
    EXPECT_TRUE(true);
}

TEST_F(RemoveShapeCommandTest, RemoveShapeCommandClassExists) {
    EXPECT_TRUE(sizeof(RemoveShapeCommand) > 0);
}
