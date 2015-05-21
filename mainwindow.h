#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /* MENU BAR FUNCTIONS */
    void on_actionOpen_triggered();
    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;

    void handleScene(QString file, string sceneFilePath);
};

#endif // MAINWINDOW_H
