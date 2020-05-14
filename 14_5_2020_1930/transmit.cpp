#include "transmit.h"

transmit::transmit(QObject *parent)
    : QObject(parent)
{
}

void transmit::init(bool *data, bool *sendPin, QMutex *mtx) {

    sent = sendPin;
    dataBits = data;
    mutex = mtx;
}

void transmit::sendData(QByteArray data) {


    //mutex->lock();
    QBitArray localDataBits;
    localDataBits = byteToBits(data);

    //qDebug() << "Sending data to thread";


    for (int j = 0; j<data.count()*8; j++)
        {

            int write = localDataBits.at(j) ? 1 : 0;
            dataBits[j] = write;
        }


    //dataBits = data;
    sent = 1;
    emit done();
    sent = 0;

    //qDebug() << "sent";
    //mutex->unlock();

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
