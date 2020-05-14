#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <QObject>
#include <QByteArray>
#include <QBitArray>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>


class transmit: public QObject
{
    Q_OBJECT
public:
    transmit(QObject *parent = 0);
    void init(bool *data, bool *sendPin, QMutex *mtx);

public slots:
    void sendData(QByteArray data);
    QBitArray byteToBits(QByteArray data);

signals:
    void done();
private:
    bool sent;
    bool *dataBits;
    QMutex *mutex;

};

#endif // TRANSMIT_H
