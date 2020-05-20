#include "transmit.h"

transmit::transmit(QObject *parent)
    : QObject(parent)
{

}

void transmit::init(bool *data, bool *cData, QWaitCondition *sendPin, QWaitCondition *receivePin,
                    QWaitCondition *senderReady, QWaitCondition *receiverAck, QWaitCondition *senderAck,
                    QMutex *mtx, bool *finish) { // Initialise all data to have address

    sent = sendPin; // sent state from transmitter
    receive = receivePin; // receive state from receiver
    charBits = cData; // char data stream
    dataBits = data; // Data stream
    mutex = mtx; // mutex from main

    // For 3 way handshake
    sReady = senderReady; // sender send ready
    rAck = receiverAck; // receiver receive ready and send back ack
    sAck = senderAck; // sender receive ack and send back ack

    complete = finish;
}
void transmit::sendData() {
    mutex->lock();

    // Transmitter ready
    sReady->wakeAll();
    // wait for receiver to received ready and ack back to transmitter
    rAck->wait(mutex);
    // Transmitter receive ack and ack back to receiver
    sAck->wakeAll();
    // Wait for receiver to be ready to receive data
    receive->wait(mutex);

    QByteArray c = charQ.dequeue();
    QByteArray data = dataQ.dequeue();

    QBitArray localDataBits;
    QBitArray localCharBits;
    localCharBits = byteToBits(c);
    localDataBits = byteToBits(data);

    for (int j = 0; j<c.count()*8; j++)
        {
            int write = localCharBits.at(j) ? 1 : 0;
            charBits[j] = write;
        }

    for (int j = 0; j<data.count()*8; j++)
        {
            int write = localDataBits.at(j) ? 1 : 0;
            dataBits[j] = write;
        }

    // Transmitter finish sending
    sent->wakeAll();
    // Wait for receiver to receive data
    receive->wait(mutex);
    mutex->unlock();

    if (!dataQ.isEmpty()) { // if thread is not empty, keep sending
        sendData();
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
void transmit::done() { // when window is closed
    if (!*complete) {
        *complete = 1;
        queue(0, 0);
    }
}
