#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)

{
    sCanvas = new drawing(this);
//    setCentralWidget(sCanvas);
//    sCanvas->open();
//    createAction();
//    createMenu();
//    this->setWindowTitle("Draw");
//    resize(450, 450);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::type(bool type) {

//    if (!type) {
//        setCentralWidget(sCanvas);
//        sCanvas->open();
//        createAction();
//        createMenu();
//        this->setWindowTitle("Draw");
//        resize(450, 450);
//    }

//    else if (type) {
//        setCentralWidget(sCanvas);
//        sCanvas->open();
//        this->setWindowTitle("Receive");
//        resize(450, 450);
//    }
//}

//void MainWindow::changePenColour() {

//    QColor userColour = QColorDialog::getColor(sCanvas->getPenColour());
//    if (userColour.isValid()) {
//        sCanvas->colourSelector(userColour);
//    }
//}

//void MainWindow::changePenSize()
//{
//    bool check;
//    int newSize = QInputDialog::getInt(this, tr("Size"),
//                                       tr("Enter pen size:"),
//                                       sCanvas->getPenWidth(),
//                                       1, 30, 1, &check);
//    if (check) {
//        sCanvas->widthSelector(newSize);
//    }
//}
//void MainWindow::createMenu() {

//    brushMenu = new QMenu(tr("&Brush"), this);
//    canvasMenu = new QMenu(tr("&Canvas"), this);

//    brushMenu->addAction(sizeAction);
//    brushMenu->addAction(widthAction);
//    canvasMenu->addAction(clearsAction);
//    menuBar()->addMenu(brushMenu);
//    menuBar()->addMenu(canvasMenu);
//}

//void MainWindow::createAction() {
//    sizeAction = new QAction(tr("&Colour"), this);
//    connect(sizeAction, &QAction::triggered, this, &MainWindow::changePenColour);

//    widthAction = new QAction(tr("&Width"), this);
//    connect(widthAction, &QAction::triggered, this, &MainWindow::changePenSize);

//    clearsAction = new QAction(tr("&Clear Screen"), this);
//    connect(clearsAction, &QAction::triggered, sCanvas, &drawing::clear);
//}

