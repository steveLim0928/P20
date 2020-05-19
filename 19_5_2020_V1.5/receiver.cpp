#include "receiver.h"

receiver::receiver(QObject *parent)
    : QObject(parent)
{}

void receiver::init(bool *Data, bool *Char, bool *sendPin, bool *receivePin,
                    bool *senderReady, bool *receiverAck, bool *senderAck,
                    QMutex *mtx, bool *finish) { // Initialise all data to have address

    incomeData = Data; // data stream
    incomeChar = Char; //  char data stream
    sent = sendPin; // sent state from transmitter
    receive = receivePin; // receive state from receiver
    mutex = mtx; // mutex from main

    sReady = senderReady;
    rAck = receiverAck;
    sAck = senderAck;

    complete = finish;
}
void receiver::receiveData() {

    while(!*sReady) { // waiting for sender to send ready = 1;
        Sleep(1);
    }

    mutex->lock();
    *rAck = 1;
    mutex->unlock();

    while (!*sAck) { // waiting for sender to send ack = 1;
        Sleep(1);
    }

    while(!*sent) { // wait for sent state from transmitter to become 1
        Sleep(1);
    }
    mutex->lock();

    QBitArray readChar;
    readChar.resize(sizeof (QChar)*8); // to ensure it can hold all bits
    for (int n=0; n<readChar.size(); n++)
        {

            bool bit = (incomeChar[n]==1) ? true : false;
            readChar.setBit(n, bit);
        }

    // deserialise
    QByteArray rChar = bittobyte(readChar);
    QChar c;
    c = deserial<QChar>(rChar);

    QBitArray readdata;
    // to resize readata to ensure it can read all necessary bits
    if (c == 'p') { // coords
        readdata.resize(64);
    }
    else if (c == 't') { // toggle drawing and last coords
        readdata.resize(64);
    }
    else if (c == 'w') { // pen width
        readdata.resize(32);
    }
    else if (c == 'c') { // pen colour
        readdata.resize(88);
    }
    else if (c == 'e') { // eraser
        readdata.resize(88);
    }
    else if (c == 'P') { // pen
        readdata.resize(88);
    }
    else if (c == 'r') { // resize canvas
        readdata.resize(64);
    }
    else if (c == 'q') { // canvas colour
        readdata.resize(88);
    }
    else if (c == 'C') { // clear canvas
        readdata.resize(32);
    }

    qDebug() << "Receiver: Reading Data...";
    for (int n=0; n<readdata.size(); n++)
        {

            bool bit = (incomeData[n]==1) ? true : false;
            readdata.setBit(n, bit);
        }

    QByteArray rdata = bittobyte(readdata);

    if (c == 'p') { // coords
        emit sendPosition(rdata);
        qDebug() << "Canvas coordinate updated";
    }
    else if (c == 't') { // toggle drawing and last coords
        emit sendToggle(rdata);
        qDebug() << "Drawing toggle";
    }
    else if (c == 'w') { // pen width
        emit sendPenWidth(rdata);
        qDebug() << "Pen width changed";
    }
    else if (c == 'c') { // pen colour
        emit sendPenColour(rdata);
        qDebug() << "Pen colour changed";
    }
    else if (c == 'e') { // eraser
        emit sendEraser(rdata);
        qDebug() << "Eraser selected";
    }
    else if (c == 'P') { // eraser
        emit sendPen(rdata);
        qDebug() << "Pen selected";
    }
    else if (c == 'r') { // reisze canvas
        emit sendCanvasSize(rdata);
        qDebug() << "Canvas resized";
    }
    else if (c == 'q') { // reisze canvas
        emit sendCanvasColour(rdata);
        qDebug() << "Canvas colour changed";
    }
    else if (c == 'C') { // clear canvas
        emit sendClear(rdata);
        qDebug() << "Clear canvas";
    }

    *receive = 1; // notify transmitter that receiver has received datastream
    *rAck = 0;
    mutex->unlock();

    while (*sent) { // wait for sent state from transmitter to become 0, indicate reset
        Sleep(1);
    }
    mutex->lock();
    *receive = 0; // reset back receive state
    mutex->unlock();

    if (!*complete) {
        emit done(); // call same function again, prepare for another receive when transmitter is ready
    }
    else {
        qDebug() << "Cleaning done";
        emit threadQuit();
    }
}
QByteArray receiver::bittobyte(QBitArray convert)
{
    QByteArray bytes;
    bytes.resize(convert.count()/8+1);
    bytes.fill(0);

    for(int b=0; b<convert.count(); ++b)
    {
        bytes[b/8] = ( bytes.at(b/8) | ((convert[b]?1:0)<<(7-(b%8))));
    }
    return bytes;
}
