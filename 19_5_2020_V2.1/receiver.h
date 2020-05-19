#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QMutex>
#include <QBitArray>
#include <QByteArray>
#include <QDataStream>
#include <QWaitCondition>
#include <QDebug>

class receiver : public QObject
{
    Q_OBJECT
public:
    receiver(QObject *parent = 0);
    void init(bool *Data, bool *Char, QWaitCondition *sendPin, QWaitCondition *receivePin, QWaitCondition *senderReady, QWaitCondition *receiverAck, QWaitCondition *senderAck, QMutex *mtx, bool *finish);
    QByteArray bittobyte(QBitArray convert);

public slots:
    void receiveData();

signals:
    void sendPosition(QByteArray data);
    void sendToggle(QByteArray data);
    void sendLastPt(QByteArray data);
    void sendPenWidth(QByteArray data);
    void sendPenColour(QByteArray data);
    void sendCanvasSize(QByteArray data);
    void sendCanvasColour(QByteArray data);
    void sendEraser(QByteArray data);
    void sendPen(QByteArray data);
    void sendClear(QByteArray data);
    void done();

private:
    bool *incomeData;
    bool *incomeChar;
    QWaitCondition *sent;
    QWaitCondition *receive;
    QMutex *mutex;

    QWaitCondition *sReady;
    QWaitCondition *rAck;
    QWaitCondition *sAck;

    bool *complete;



};

template <typename inType>

inType deserial(QByteArray input) {

    QDataStream deserial(input);
    inType point;
    deserial >> point;
    return point;

}

#endif // RECEIVER_H
