#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QBitArray>
#include <QByteArray>
#include <QDataStream>

//#include <QTest>
#include <windows.h>

class receiver : public QObject
{
    Q_OBJECT
public:
    receiver(QObject *parent = 0);
    void init(bool *Data, bool *Char, bool *sendPin, bool *receivePin, bool *senderReady, bool *receiverAck, bool *senderAck, QMutex *mtx);
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
    bool *sent;
    bool *receive;
    QMutex *mutex;

    bool *sReady;
    bool *rAck;
    bool *sAck;

};

#endif // RECEIVER_H
