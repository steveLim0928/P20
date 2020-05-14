#include "receiver.h"

receiver::receiver(QObject *parent)
    : QObject(parent)
{
}

void receiver::init(bool *Data, bool *sendPin, QMutex *mtx) {

    incomeData = Data;
    sent = sendPin;
    mutex = mtx;
}

void receiver::receiveData() {

    //qDebug() << sent;
    //mutex->lock();
//    for (int i = 0; i < 10; i ++) {
//        qDebug() << incomeData[i];
//    }

    QBitArray readdata;
    readdata.resize(64);
    for (int n=0; n<readdata.size(); n++)
        {

            bool bit = (incomeData[n]==1) ? true : false;
            readdata.setBit(n, bit);
        }

    QByteArray rdata = bittobyte(readdata);

    if (sent) {
        emit sendPosition(rdata);
        qDebug() << "Data received";

    }
    //mutex->unlock();

    //emit done();
}

QByteArray receiver::bittobyte(QBitArray convert)                           //code developed from: https://wiki.qt.io/Working_with_Raw_Data
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
