#include <QDataStream>
#include <QByteArray>

template <class inType>
class serialise { // serialise any data type to QByteArry and deserialise QByteArry to any data type

public:

    QByteArray serial(inType input);
    inType deserial(QByteArray input);
};

template <class inType> QByteArray serialise<inType>::serial(inType input) {

    QByteArray buffer;
    QDataStream serial(&buffer, QIODevice::WriteOnly);
    serial << input;
    return buffer;
}

template <class inType> inType serialise<inType>::deserial(QByteArray input) {

    QDataStream deserial(input);
    inType point;
    deserial >> point;
    return point;

}
