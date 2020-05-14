#include "mainwindow.h"

#include <QApplication>
#include <QObject>
#include "drawing.h"
#include "transmit.h"
#include "receiver.h"
#include <QThread>

bool sendState = 0;
bool dataStream[100];

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow uWin;
    //MainWindow rWin;

    //uWin.type(0);
    //rWin.type(1);

    //QObject::connect(&uWin, SIGNAL(drawing::mousePress()), &rWin, SLOT(drawing::toggleDraw()));
    //QObject::connect(&uWin, SIGNAL(&drawing::sendData()), &rWin, SLOT(&drawing::onReceivePos()));
    //QObject::connect(&uWin, SIGNAL(&drawing::lastPt()), &rWin, SLOT(&drawing::onReceiveLastPt()));

//    uWin.show();
//    rWin.show();

    drawing userWin;
    drawing receiveWin;

    userWin.resize(450, 450);
    receiveWin.resize(450, 450);

    userWin.type(false);
    receiveWin.type(true);

    userWin.setWindowTitle("Draw");
    receiveWin.setWindowTitle("Receive");

    QThread transmitThread;
    QThread receiveThread;

    QMutex mutex;

    transmit *transmitter = new transmit();
    receiver *receive = new receiver();

    transmitter->moveToThread(&transmitThread);
    receive->moveToThread(&receiveThread);

    transmitter->init(dataStream, &sendState, &mutex);
    receive->init(dataStream, &sendState, &mutex);

    QObject::connect(&userWin, &drawing::mousePress, &receiveWin, &drawing::toggleDraw);
    QObject::connect(&userWin, &drawing::sendData, transmitter, &transmit::sendData);
    //QObject::connect(&userWin, &drawing::sendData, &receiveWin, &drawing::onReceivePos);
    QObject::connect(&userWin, &drawing::lastPt, &receiveWin, &drawing::onReceiveLastPt);
    QObject::connect(&userWin, &drawing::widthSignal, &receiveWin, &drawing::onPenSize);
    QObject::connect(&userWin, &drawing::clearSignal, &receiveWin, &drawing::onClear);
    QObject::connect(&userWin, &drawing::colourSignal, &receiveWin, &drawing::onPenColour);
    QObject::connect(&userWin, &drawing::canvasSizeSignal, &receiveWin, &drawing::onResizeCanvas);

    QObject::connect(&receiveThread, &QThread::started, receive, &receiver::receiveData);
    QObject::connect(transmitter, &transmit::done, receive, &receiver::receiveData);
    QObject::connect(receive, &receiver::sendPosition, &receiveWin, &drawing::onReceivePos);

    transmitThread.start();
    receiveThread.start();

    userWin.show();
    receiveWin.show();

    a.exec();
    transmitThread.quit();
    transmitThread.terminate();
    receiveThread.quit();
    receiveThread.terminate();
}
