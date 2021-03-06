#include "transmit.h"

transmit::transmit(QObject *parent)
    : QObject(parent)
{

}

void transmit::init(bool *data, bool *cData, bool *sendPin, bool *receivePin,
                    bool *senderReady, bool *receiverAck, bool *senderAck,
                    QMutex *mtx, bool *finish) { // Initialise all data to have address

    sent = sendPin; // sent state from transmitter
    receive = receivePin; // receive state from receiver
    charBits = cData; // char data stream
    dataBits = data; // Data stream
    mutex = mtx; // mutex from main
    sReady = senderReady; // sender send ready
    rAck = receiverAck; // receiver receive ready and send back ack
    sAck = senderAck; // sender receive ack and send back ack

    complete = finish;
}
void transmit::sendData() {

    mutex->lock();
    qDebug() << "Initiate 3 way handshake";

    *sReady = 1;

    mutex->unlock();

    while (!*rAck) { // waiting for receiver to send ack = 1;
        Sleep(1);
    }
    mutex->lock();
    *sAck = 1;
    qDebug() << "";
    mutex->unlock();

    QByteArray c = charQ.dequeue();
    QByteArray data = dataQ.dequeue();


    while (*receive) { // wait for receive state from receiver to become 0
        Sleep(1);
    }

    mutex->lock();
    QBitArray localDataBits;
    QBitArray localCharBits;
    localCharBits = byteToBits(c);
    localDataBits = byteToBits(data);

    qDebug() << "Transmitter: Sending Char...";
    Sleep(1);
    for (int j = 0; j<c.count()*8; j++)
        {

            int write = localCharBits.at(j) ? 1 : 0;
            charBits[j] = write;
        }

    qDebug() << "Transmitter: Sending Data...";

    for (int j = 0; j<data.count()*8; j++)
        {

            int write = localDataBits.at(j) ? 1 : 0;
            dataBits[j] = write;
        }

    qDebug() << "Transmitter: Data SENT!";

    *sent = 1; // notify receiver that transmitter has sent the datastream

    *sReady = 0;
    *sAck = 0;
    mutex->unlock();
    while(!*receive) { // wait for receive state from receiver to become 1, indicate received
        Sleep(1);
    }
    mutex->lock();
    *sent = 0; // reset back sent state
    mutex->unlock();

    while(*receive) { // wait for receive state from receiver to become 0, indicate reset
        Sleep(1);
    }
    if (!dataQ.isEmpty()) { // if thread is not empty, keep sending
        sendData();
    }
    if (*complete) {
        threadQuit();
    }

}
QBitArray transmit::byteToBits(QByteArray data) {

    QBitArray bits;

    for(int i = 0; i < data.count(); ++i) {
      for(int b = 0; b < 8; b++) {
        bits.setBit( i * 8 + b, data.at(i) & (1 << (7 - b)) );
      }
    }
    return bits;
}

void transmit::queue(QByteArray data, QByteArray c) { // thread safe queue

    mutex->lock();
    // queue each element
    dataQ.enqueue(data);
    charQ.enqueue(c);

    // transmit the queue
    mutex->unlock();
    sendData();
}
void transmit::done() {
    if (!*complete) {
        *complete = 1;
        //qDebug() << "Cleaning";
        queue(0, 0);
    }
}
