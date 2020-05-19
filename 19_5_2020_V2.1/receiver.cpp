#include "receiver.h"

receiver::receiver(QObject *parent)
    : QObject(parent)
{}

void receiver::init(bool *Data, bool *Char, QWaitCondition *sendPin, QWaitCondition *receivePin,
                    QWaitCondition *senderReady, QWaitCondition *receiverAck, QWaitCondition *senderAck,
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

    mutex->lock();

    // Wait for transmitter to send ready
    sReady->wait(mutex);

    // Send ack to transmitter
    rAck->wakeAll();

    // Wait for transmitter to send ack
    sAck->wait(mutex);

    // Receiver ready to receive
    receive->wakeAll();

    // Wait for transmitter to finish sending data
    sent->wait(mutex);

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

    for (int n=0; n<readdata.size(); n++)
        {

            bool bit = (incomeData[n]==1) ? true : false;
            readdata.setBit(n, bit);
        }

    QByteArray rdata = bittobyte(readdata);

    if (c == 'p') { // coords
        emit sendPosition(rdata);
    }
    else if (c == 't') { // toggle drawing and last coords
        emit sendToggle(rdata);
    }
    else if (c == 'w') { // pen width
        emit sendPenWidth(rdata);
    }
    else if (c == 'c') { // pen colour
        emit sendPenColour(rdata);
    }
    else if (c == 'e') { // eraser
        emit sendEraser(rdata);
    }
    else if (c == 'P') { // eraser
        emit sendPen(rdata);
    }
    else if (c == 'r') { // reisze canvas
        emit sendCanvasSize(rdata);
    }
    else if (c == 'q') { // reisze canvas
        emit sendCanvasColour(rdata);
    }
    else if (c == 'C') { // clear canvas
        emit sendClear(rdata);
    }

    // Receiver finish reading
    receive->wakeAll();

    mutex->unlock();

    if (!*complete) { // if window not close, prepare to receive
        emit done(); // call same function again, prepare for another receive when transmitter is ready
    }
    else { // finish ending receive thread
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
