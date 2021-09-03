#ifndef MIFARE_H
#define MIFARE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QRegExp>
#include <QJsonArray>
#include <QJsonObject>

class Mifare : public QThread
{
    Q_OBJECT

public:
    explicit Mifare(QObject *parent = nullptr);
    Q_INVOKABLE bool connectSerial();
    Q_INVOKABLE void getTag();
    Q_INVOKABLE void persistentTag();
    Q_INVOKABLE void persistentLogin(int=0,int=0,QString="A",QString="FFFFFFFFFFFF");
    Q_INVOKABLE void write(int=1,QString="0000000000000000000000000000000");
    Q_INVOKABLE void read(int=0);
    Q_INVOKABLE void login(int=0,QString="A", QString="FFFFFFFFFFFF");
    ~Mifare();

signals:
    void successRead(QString tag);
    void errRead();
    void getTagChanged(QString tag);
    void dataChanged(QByteArray data);
    void successLoginChanged();

public slots:
    void readyRead();

private:
    char sum(QByteArray);
    void send(QByteArray);
    QSerialPort *_serial;
    QString _readData;

    unsigned int m_index_port;
    int m_connected = 0;
    bool m_block = false;
    QString m_flag;
//    int _sector;
};

#endif // MIFARE_H
