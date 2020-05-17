#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <QObject>
#include <QByteArray>
#include <QBitArray>
#include <QDebug>
#include <QMutex>
#include <QDataStream>

//#include <QTest>
#include <windows.h>

#include <QQueue>

class transmit: public QObject
{
    Q_OBJECT
public:
    transmit(QObject *parent = 0);
    void init(bool *data, bool *cData, bool *sendPin, bool *receivePin, bool *senderReady, bool *receiverAck, bool *senderAck, QMutex *mtx);

public slots:
    void sendData();
    void queue(QByteArray data, QChar c);


signals:
    void done();
private:
    bool *sent;
    bool *receive;
    bool *dataBits;
    bool *charBits;
    QMutex *mutex;

    bool *sReady;
    bool *rAck;
    bool *sAck;

    QBitArray byteToBits(QByteArray data);

    QQueue<QByteArray> dataQ;
    QQueue<QChar> charQ;

};

#endif // TRANSMIT_H
