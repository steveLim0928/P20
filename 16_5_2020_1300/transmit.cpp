#include "transmit.h"

transmit::transmit(QObject *parent)
    : QObject(parent)
{}

void transmit::init(bool *data, bool *cData, bool *sendPin, bool *receivePin, bool *senderReady, bool *receiverAck, bool *senderAck, QMutex *mtx) { // Initialise all data to have address

    sent = sendPin; // sent state from transmitter
    receive = receivePin; // receive state from receiver
    charBits = cData; // char data stream
    dataBits = data; // Data stream
    mutex = mtx; // mutex from main
    sReady = senderReady; // sender send ready
    rAck = receiverAck; // receiver receive ready and send back ack
    sAck = senderAck; // sender receive ack and send back ack
}
void transmit::sendData(QByteArray data, QChar c) {

    QDebug debug = qDebug(); // custom qdebug to allow printing without new line

    mutex->lock();
    qDebug() << "";
    qDebug() << "Initiate 3 way handshake";
    *sReady = 1;
    qDebug() << "Transmitter: Ready";
    mutex->unlock();

    while (!*rAck) { // waiting for receiver to send ack = 1;
        //debug << "";
        //QTest::qSleep(1);
        Sleep(1);
    }
    mutex->lock();
    *sAck = 1;
    qDebug() << "Transmitter: receiver Ack";
    qDebug() << "";
    mutex->unlock();
    //QTest::qSleep(1);
    Sleep(1);

    // Serialise char from user window
    QByteArray serialiseChar;
    QDataStream serial(&serialiseChar, QIODevice::WriteOnly);
    serial << c;

    while (*receive) { // wait for receive state from receiver to become 0
        //debug << "";
        //QTest::qSleep(1);
        Sleep(1);
    }

    mutex->lock();
    QBitArray localDataBits;
    QBitArray localCharBits;
    localCharBits = byteToBits(serialiseChar);
    localDataBits = byteToBits(data);

    qDebug() << "Transmitter: Sending Char...";
    for (int j = 0; j<serialiseChar.count()*8; j++)
        {

            int write = localCharBits.at(j) ? 1 : 0;
            charBits[j] = write;
        }
    qDebug() << "Transmitter: Type "<< c << " SENT!";

    qDebug() << "Transmitter: Sending Data...";
    qDebug() << "Transmitter: Data Size: " << data.count()*8;

    for (int j = 0; j<data.count()*8; j++)
        {

            int write = localDataBits.at(j) ? 1 : 0;
            dataBits[j] = write;
        }

    qDebug() << "Transmitter: Data SENT!";

    *sent = 1; // notify receiver that transmitter has sent the datastream
    qDebug() << "Transmitter: Sent state set";

    *sReady = 0;
    *sAck = 0;
    mutex->unlock();
    while(!*receive) { // wait for receive state from receiver to become 1, indicate received
        //debug << "";
        //QTest::qSleep(1);
        Sleep(1);
    }
    mutex->lock();
    *sent = 0; // reset back sent state
    qDebug() << "Transmitter: Sent state reset";
    mutex->unlock();

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
