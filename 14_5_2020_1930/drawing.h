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
#include <QPushButton>
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

#include <QPushButton>

class drawing : public QWidget
{
    Q_OBJECT
public:
    drawing(QWidget *parent = 0);

    // Shared
    void open();
    void type(bool in);
    void resizeImage(QImage *canvasTemplate, const QSize newSize);

    // Unused

    //QColor getPenColour() {return penColour;}
    //int getPenWidth() {return penWidth;}

    //void colourSelector(const QColor &userColour);
    //void widthSelector(const int &width);

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
    void getResizeData();
    void close() {dialog->close();}

    // Receiver
    void toggleDraw(QByteArray data);
    void onReceivePos(QByteArray data);
    void onReceiveLastPt(QByteArray data);
    void onClear();
    void onPenColour(QByteArray data);
    void onPenSize(QByteArray data);
    void onResizeCanvas(QByteArray data);

    // Shared
    void clear();


signals:
    void sendData(const QByteArray data);
    void mousePress(const QByteArray data);
    void lastPt(const QByteArray data);

    void widthSignal(const QByteArray data);
    void colourSignal(const QByteArray data);
    void clearSignal(const QByteArray data);

    void canvasSizeSignal(const QByteArray data);

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
    int penWidth = 2;

    int receiveDraw = 0;

    bool receiver = false;

    QAction *sizeAction;
    QAction *widthAction;
    QAction *clearsAction;
    QAction *canvasAction;

    QSize windowSize;
    QLineEdit *width;
    QLineEdit *height;
    QDialog *dialog;
};

#endif // DRAWING_H
