#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <QObject>
#include <QByteArray>
#include <QBitArray>
#include <QMutex>
#include <QDataStream>

#include <QWaitCondition>
#include <QQueue>
#include <QDebug>

class transmit: public QObject
{
    Q_OBJECT
public:
    transmit(QObject *parent = 0);
    void init(bool *data, bool *cData, QWaitCondition *sendPin, QWaitCondition *receivePin, QWaitCondition *senderReady, QWaitCondition *receiverAck, QWaitCondition *senderAck, QMutex *mtx, bool *finish);

public slots:
    void sendData();
    void queue(QByteArray data, QByteArray c);
    void done();

private:
    QWaitCondition *sent;
    QWaitCondition *receive;
    bool *dataBits;
    bool *charBits;
    QMutex *mutex;

    QWaitCondition *sReady;
    QWaitCondition *rAck;
    QWaitCondition *sAck;

    QBitArray byteToBits(QByteArray data);

    QQueue<QByteArray> dataQ;
    QQueue<QByteArray> charQ;

    bool *complete;
};

template <typename inType>

QByteArray serial(inType input) {

    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << input;
    return buffer;
}

#endif // TRANSMIT_H
