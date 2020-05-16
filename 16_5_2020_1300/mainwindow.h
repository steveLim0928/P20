#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include "drawing.h"
#include <QMenu>
#include <QAction>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void changePenColour();
    void changePenSize();

    void type(bool type);


private:
    Ui::MainWindow *ui;

    drawing *sCanvas;
    drawing *rCanvas;

    QMenu *brushMenu;
    QMenu *canvasMenu;

    QAction *sizeAction;
    QAction *widthAction;
    QAction *clearsAction;

    void createMenu();
    void createAction();
};
#endif // MAINWINDOW_H
