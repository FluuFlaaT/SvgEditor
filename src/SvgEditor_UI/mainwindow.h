#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

<<<<<<< Updated upstream
=======
#include <QFileDialog>

// TODO: 将画布实现独立
class CanvasView;

>>>>>>> Stashed changes
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

<<<<<<< Updated upstream
private:
    Ui::MainWindow *ui;
=======
private slots:
    void newFile();
    void openFile();
    void saveFile();

private:
    Ui::MainWindow *ui;

    // QAction 用户行为
    QAction *openAction;
    QAction *saveAction;
    QAction *exitAction;

    // QMenu 菜单
    QMenu *fileMenu;
    QMenu *editMenu;

    QToolBar *drawingToolBar;

    CanvasView *canvasView;

    // 函数实现
    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBar();
>>>>>>> Stashed changes
};

#endif // MAINWINDOW_H