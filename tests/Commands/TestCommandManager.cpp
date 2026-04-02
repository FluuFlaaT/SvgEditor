#include <gtest/gtest.h>
#include <Commands/Command.h>
#include <Commands/CommandManager.h>
#include <memory>

class MockCommand : public Command {
public:
    MockCommand(const QString& desc, bool executeResult = true, bool undoResult = true)
        : Command(desc), m_executeResult(executeResult), m_undoResult(undoResult), m_executed(false), m_undone(false) {}
    
    bool execute() override {
        m_executed = true;
        return m_executeResult;
    }
    
    bool undo() override {
        m_undone = true;
        return m_undoResult;
    }
    
    bool wasExecuted() const { return m_executed; }
    bool wasUndone() const { return m_undone; }
    
private:
    bool m_executeResult;
    bool m_undoResult;
    bool m_executed;
    bool m_undone;
};

class CommandManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        cmdMgr = CommandManager::instance();
        cmdMgr->clear();
    }
    
    void TearDown() override {
        cmdMgr->clear();
    }
    
    CommandManager* cmdMgr;
};

TEST_F(CommandManagerTest, SingletonInstanceNotNull) {
    EXPECT_NE(CommandManager::instance(), nullptr);
}

TEST_F(CommandManagerTest, InitialStateCannotUndo) {
    EXPECT_FALSE(cmdMgr->canUndo());
}

TEST_F(CommandManagerTest, InitialStateCannotRedo) {
    EXPECT_FALSE(cmdMgr->canRedo());
}

TEST_F(CommandManagerTest, ExecuteCommandEnablesUndo) {
    auto cmd = std::make_unique<MockCommand>("Test command");
    cmdMgr->executeCommand(std::move(cmd));
    EXPECT_TRUE(cmdMgr->canUndo());
}

TEST_F(CommandManagerTest, ExecuteCommandClearsRedoStack) {
    auto cmd1 = std::make_unique<MockCommand>("Command 1");
    cmdMgr->executeCommand(std::move(cmd1));
    cmdMgr->undo();
    EXPECT_TRUE(cmdMgr->canRedo());
    
    auto cmd2 = std::make_unique<MockCommand>("Command 2");
    cmdMgr->executeCommand(std::move(cmd2));
    EXPECT_FALSE(cmdMgr->canRedo());
}

TEST_F(CommandManagerTest, UndoDisablesUndoWhenStackEmpty) {
    auto cmd = std::make_unique<MockCommand>("Test command");
    cmdMgr->executeCommand(std::move(cmd));
    EXPECT_TRUE(cmdMgr->canUndo());
    cmdMgr->undo();
    EXPECT_FALSE(cmdMgr->canUndo());
}

TEST_F(CommandManagerTest, UndoAfterRedoEnablesRedo) {
    auto cmd1 = std::make_unique<MockCommand>("Command 1");
    cmdMgr->executeCommand(std::move(cmd1));
    cmdMgr->undo();
    EXPECT_TRUE(cmdMgr->canRedo());
    cmdMgr->redo();
    EXPECT_FALSE(cmdMgr->canRedo());
}

TEST_F(CommandManagerTest, ClearResetsBothStacks) {
    auto cmd1 = std::make_unique<MockCommand>("Command 1");
    auto cmd2 = std::make_unique<MockCommand>("Command 2");
    cmdMgr->executeCommand(std::move(cmd1));
    cmdMgr->executeCommand(std::move(cmd2));
    cmdMgr->undo();
    
    cmdMgr->clear();
    
    EXPECT_FALSE(cmdMgr->canUndo());
    EXPECT_FALSE(cmdMgr->canRedo());
}

TEST_F(CommandManagerTest, UndoTextReturnsLastCommandDescription) {
    auto cmd = std::make_unique<MockCommand>("My Test Command");
    cmdMgr->executeCommand(std::move(cmd));
    EXPECT_EQ(cmdMgr->undoText(), QString("My Test Command"));
}

TEST_F(CommandManagerTest, RedoTextAfterUndo) {
    auto cmd = std::make_unique<MockCommand>("Redo Test");
    cmdMgr->executeCommand(std::move(cmd));
    cmdMgr->undo();
    EXPECT_EQ(cmdMgr->redoText(), QString("Redo Test"));
}

TEST_F(CommandManagerTest, MultipleCommandsUndoInOrder) {
    auto cmd1 = std::make_unique<MockCommand>("First");
    auto cmd2 = std::make_unique<MockCommand>("Second");
    auto cmd3 = std::make_unique<MockCommand>("Third");
    
    cmdMgr->executeCommand(std::move(cmd1));
    cmdMgr->executeCommand(std::move(cmd2));
    cmdMgr->executeCommand(std::move(cmd3));
    
    EXPECT_TRUE(cmdMgr->canUndo());
    EXPECT_EQ(cmdMgr->undoText(), QString("Third"));
    
    cmdMgr->undo();
    EXPECT_EQ(cmdMgr->undoText(), QString("Second"));
    
    cmdMgr->undo();
    EXPECT_EQ(cmdMgr->undoText(), QString("First"));
}

TEST_F(CommandManagerTest, ExecuteCommandFailurePath) {
    auto cmd = std::make_unique<MockCommand>("Fail", false, true);
    bool result = cmdMgr->executeCommand(std::move(cmd));
    EXPECT_FALSE(result);
    EXPECT_FALSE(cmdMgr->canUndo());
}

TEST_F(CommandManagerTest, UndoFailurePath) {
    auto cmd = std::make_unique<MockCommand>("UndoFail", true, false);
    cmdMgr->executeCommand(std::move(cmd));
    bool undoResult = cmdMgr->undo();
    EXPECT_FALSE(undoResult);
    EXPECT_FALSE(cmdMgr->canRedo());
}

TEST_F(CommandManagerTest, RedoFailurePath) {
    auto cmd = std::make_unique<MockCommand>("RedoFail", true, false);
    cmdMgr->executeCommand(std::move(cmd));
    cmdMgr->undo();
    bool redoResult = cmdMgr->redo();
    EXPECT_FALSE(redoResult);
}

TEST_F(CommandManagerTest, UndoTextReturnsEmptyWhenStackEmpty) {
    EXPECT_EQ(cmdMgr->undoText(), QString());
}

TEST_F(CommandManagerTest, RedoTextReturnsEmptyWhenStackEmpty) {
    EXPECT_EQ(cmdMgr->redoText(), QString());
}

TEST_F(CommandManagerTest, RedoTextReturnsEmptyAfterClear) {
    auto cmd = std::make_unique<MockCommand>("Test");
    cmdMgr->executeCommand(std::move(cmd));
    cmdMgr->undo();
    cmdMgr->clear();
    EXPECT_EQ(cmdMgr->redoText(), QString());
}
