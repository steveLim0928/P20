#ifndef DRAWING_H
#define DRAWING_H
#include <QImage>
#include <QString>
#include <QWidget>
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

#include "serialise.h"
#include <QCloseEvent>

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
    void getText();
    void changeTextSize();
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
    void onTextPressed(QByteArray data);
    void onSetText(QByteArray data);
    void onSetTextSize(QByteArray data);
    void onResizeCanvas(QByteArray data);
    void onCanvasColour(QByteArray data);
    void onEraser(QByteArray data);
    void onPen(QByteArray data);

    // Shared
    void clear();
    void colourCanvas();


signals:
    void sendData(const QByteArray data, const QByteArray c);
    void windowClose();

private:
    // Sender
    void createMenu();
    void createAction();

    // Shared
    void draw(const QPoint &pos);
    void text(const QPoint &pos);

    void closeEvent (QCloseEvent *event) override;

    QImage canvasTemplate;
    QPoint lastPoint;

    bool paint = false;

    QColor penColour = {0,0,0};
    QColor penNewColour = {0,0,0};
    QColor canvasNewColour = {255, 255, 255};
    QColor canvasColour = {255, 255, 255};
    int penWidth = 2;

    QString displayText;
    bool texting = 0;
    int fontSize = 10;

    int receiveDraw = 0;

    bool receiver = false;

    QAction *penColourAction;
    QAction *widthAction;
    QAction *clearsAction;
    QAction *canvasAction;
    QAction *canvasColourAction;
    QAction *eraserAction;
    QAction *penAction;
    QAction *textAction;
    QAction *textSizeAction;

    QSize windowSize = {450, 450};
    QLineEdit *width;
    QLineEdit *height;
    QDialog *dialog;

    serialise<QPoint> pointSerial;
    serialise<QColor> colourSerial;
    serialise<int> intSerial;
    serialise<QSize> sizeSerial;
    serialise<QChar> charSerial;
    serialise<QString> stringSerial;
};

#endif // DRAWING_H
