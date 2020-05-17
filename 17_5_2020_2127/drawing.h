#ifndef DRAWING_H
#define DRAWING_H
#include <QImage>
#include <QString>
#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QPoint>
#include <QDataStream>
#include <QBitArray>

#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QAction>
#include <QColorDialog>
#include <QInputDialog>

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QObject>

class drawing : public QWidget
{
    Q_OBJECT
public:
    drawing(QWidget *parent = 0);

    // Shared
    void open();
    void type(bool in);
    void resizeImage(QImage *canvasTemplate, const QSize newSize);

protected:
    // Sender
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // Shared
    void paintEvent(QPaintEvent *event) override;

public slots:
    //Sender
    void changePenColour();
    void changePenSize();
    void changeCanvasSize();
    void changeCanvasColour();
    void getResizeData();
    void eraser();
    void pen();
    void close() {dialog->close();}

    // Receiver
    void toggleDraw(QByteArray data);
    void onReceivePos(QByteArray data);
    void onClear();
    void onPenColour(QByteArray data);
    void onPenSize(QByteArray data);
    void onResizeCanvas(QByteArray data);
    void onCanvasColour(QByteArray data);
    void onEraser(QByteArray data);
    void onPen(QByteArray data);

    // Shared
    void clear();
    void colourCanvas();


signals:
    void sendData(const QByteArray data, const QChar c);

private:
    // Sender
    void createMenu();
    void createAction();

    // Shared
    void draw(const QPoint &pos);


    QImage canvasTemplate;
    QPoint lastPoint;

    bool paint = false;

    QColor penColour = {0,0,0};
    QColor penNewColour = {0,0,0};
    QColor canvasNewColour = {255, 255, 255};
    QColor canvasColour = {255, 255, 255};
    int penWidth = 2;

    int receiveDraw = 0;

    bool receiver = false;

    QAction *penColourAction;
    QAction *widthAction;
    QAction *clearsAction;
    QAction *canvasAction;
    QAction *canvasColourAction;
    QAction *eraserAction;
    QAction *penAction;

    QSize windowSize = {450, 450};
    QLineEdit *width;
    QLineEdit *height;
    QDialog *dialog;
};

#endif // DRAWING_H
