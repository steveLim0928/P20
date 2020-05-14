#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include <QBitArray>
#include <QByteArray>

class receiver : public QObject
{
    Q_OBJECT
public:
    receiver(QObject *parent = 0);
    void init(bool *Data, bool *sendPin, QMutex *mtx);
    QByteArray bittobyte(QBitArray convert);

public slots:
    void receiveData();

signals:
    void sendPosition(QByteArray data);
    void done();

private:
    bool *incomeData;
    bool sent;
    QMutex *mutex;

};

#endif // RECEIVER_H
