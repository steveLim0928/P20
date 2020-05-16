#include "mainwindow.h"

#include <QApplication>
#include <QObject>
#include <QThread>

#include "drawing.h"
#include "transmit.h"
#include "receiver.h"

bool sendState = 0; // transmitter state
bool receiveState = 0; // receiver state
// 'transmission medium'
bool charStream[8];
bool dataStream[88];
// 3 way handshake
bool senderReady = 0;
bool receiverAck = 0;
bool senderAck = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    drawing userWin;
    drawing receiveWin;

    userWin.resize(650, 650);
    receiveWin.resize(650, 650);

    userWin.type(false); // transmitter window
    receiveWin.type(true); // receiver window

    userWin.setWindowTitle("Draw");
    receiveWin.setWindowTitle("Receive");

    QThread transmitThread;
    QThread receiveThread;

    QMutex mutex;

    transmit *transmitter = new transmit();
    receiver *receive = new receiver();

    // move each object to their threads
    transmitter->moveToThread(&transmitThread);
    receive->moveToThread(&receiveThread);

    // Initialise by passing address of each stuff to transmitter and receiver
    transmitter->init(dataStream, charStream, &sendState, &receiveState, &senderReady, &receiverAck, &senderAck , &mutex);
    receive->init(dataStream, charStream, &sendState, &receiveState, &senderReady, &receiverAck, &senderAck, &mutex);

    // Transmit window to transmitter
    QObject::connect(&userWin, &drawing::sendData, transmitter, &transmit::sendData);

    // to enable receiver to receive
    QObject::connect(&receiveThread, &QThread::started, receive, &receiver::receiveData);

    // Keep receiver running and waiting for data
    QObject::connect(receive, &receiver::done, receive, &receiver::receiveData);

    // connect from receiver to receive window
    QObject::connect(receive, &receiver::sendToggle, &receiveWin, &drawing::toggleDraw);
    QObject::connect(receive, &receiver::sendPosition, &receiveWin, &drawing::onReceivePos);
    QObject::connect(receive, &receiver::sendPenWidth, &receiveWin, &drawing::onPenSize);
    QObject::connect(receive, &receiver::sendClear, &receiveWin, &drawing::onClear);
    QObject::connect(receive, &receiver::sendPenColour, &receiveWin, &drawing::onPenColour);
    QObject::connect(receive, &receiver::sendEraser, &receiveWin, &drawing::onEraser);
    QObject::connect(receive, &receiver::sendPen, &receiveWin, &drawing::onPen);
    QObject::connect(receive, &receiver::sendCanvasSize, &receiveWin, &drawing::onResizeCanvas);
    QObject::connect(receive, &receiver::sendCanvasColour, &receiveWin, &drawing::onCanvasColour);

    transmitThread.start();
    receiveThread.start();

    userWin.move(250,150);
    receiveWin.move(1050,150);

    userWin.show();
    receiveWin.show();

    a.exec();

    // end Thread and mutex

    transmitThread.quit();
    transmitThread.terminate();

    receiveThread.quit();
    receiveThread.terminate();

    transmitThread.~QThread();
    receiveThread.~QThread();
    mutex.~QMutex();

    return 0;
}
