#include "mifare.h"

Mifare::Mifare(QObject *parent) : QThread(parent)
{
    connectSerial();
}

bool Mifare::connectSerial(){
    _serial = new QSerialPort();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if(_serial->open(QIODevice::ReadWrite))
            qDebug() << _serial->errorString() << endl;

        if(info.vendorIdentifier() == 0x067b && info.productIdentifier() == 0x2303)
        {
            _serial->setPortName(info.portName());
            _serial->setBaudRate(QSerialPort::Baud19200);
            _serial->setDataBits(QSerialPort::Data8);
            _serial->setParity(QSerialPort::NoParity);
            _serial->setStopBits(QSerialPort::OneStop);
            _serial->setFlowControl(QSerialPort::NoFlowControl);

            if(_serial->open(QIODevice::ReadWrite))
            {
                connect(_serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
                m_connected = true;
            }else
                qDebug() << "[CRITICAL] DISCONNECTED" << endl;
        }
    }
}

void Mifare::login(int sector,QString keyType, QString key)
{
    if(m_connected)
    {
        m_flag = "login";
        QByteArray cmd;
        char bytesCmd[5] = { 0x02, 0x01, 0x09, 0x6C, char(sector) };
        cmd.append(bytesCmd, 5);
        if(keyType.toUpper() == "A")
            cmd.append(0xAA);
        else
            cmd.append(0xBB);
        cmd.append(QByteArray::fromHex(key.toUtf8()));
        cmd.append(sum(cmd.mid(1,11)));
        cmd.append(0x03);
        send(cmd);
    }else
        throw "port not conected";
}

void Mifare::read(int block)
{
    if(m_connected)
    {
        m_flag = "data";
        char bytesCmd[4] = { 0x02, 0x01, 0x02, 0x72};
        QByteArray cmd;
        cmd.append(bytesCmd, 4);
        cmd.append(char(block));
        cmd.append(sum(cmd.mid(1,11)));
        cmd.append(0x03);
        send(cmd);
    }else
        throw "port not conected";
}

void Mifare::getTag()
{
    /* GET TAG */
    if(m_connected)
    {
        m_flag = "getTag";
        char bytesCmd[6] = { 0x02, 0x01, 0x01, 0x73, 0x73, 0x03 };
        send(bytesCmd);
    }else
        throw "port not conected";
}

void Mifare::persistentTag()
{
    /* AUTO TAG */
    if(m_connected)
    {
        m_flag = "getTag";
        char bytesCmd[6] = { 0x02, 0x01, 0x01, 0x61, 0x61, 0x03 };
        send(bytesCmd);
    }else
        throw "port not conected";
}

void Mifare::persistentLogin(int sector, int block, QString keyType, QString key)
{
    /* On board key and block */
    if(m_connected)
    {
        m_flag = "data";
        QByteArray cmd;
        char bytesCmd[4] = { 0x02, 0x01, 0x0A, 0x62 };
        cmd.append(bytesCmd, 4);
        cmd.append(char(sector));
        if(keyType.toUpper() == "A")
            cmd.append(0xAA);
        else
            cmd.append(0xBB);
        cmd.append(QByteArray::fromHex(key.toUtf8()));
        cmd.append(char(block));
        cmd.append(sum(cmd.mid(1,12)));
        cmd.append(0x03);
        send(cmd);
    }else
        throw "port not conected";
}

void Mifare::write(int block, QString data)
{
    /* write block */
    if(m_connected)
    {
        m_flag = "data";
        QByteArray cmd;
        char bytesCmd[5] = { 0x02, 0x01, 0x12, 0x77, char(block) };
        cmd.append(bytesCmd, 5);
        cmd.append(QByteArray::fromHex(data.toUtf8()));
        cmd.append(sum(cmd.mid(1,20)));
        cmd.append(0x03);
        send(cmd);
    }else
        throw "port not conected";
}

void Mifare::send(QByteArray buffer)
{
    if (_serial->isOpen()){
        _serial->write(buffer);
        _serial->flush();
        buffer.clear();
    }else
        throw "Serial is closed!";
}

char Mifare::sum(QByteArray buffer)
{
    int checkSum = 0;
    for(int i=0; (buffer.length() +1) > i; i++)
        checkSum = checkSum ^ buffer[i];

    return char(checkSum & 0xFF);
}

void Mifare::readyRead()
{
    QByteArray readData = _serial->readAll();
    _serial->clear();
    QString result = readData.toHex();


    if(m_flag == "getTag"){
        QRegExp rx("^(020004).*$");
        bool isMatchRead = rx.exactMatch(result);

        if(!isMatchRead)
            return Q_EMIT errRead();

        if(isMatchRead)
            return Q_EMIT getTagChanged(result.mid(6,8).toUpper());
    }

    if(m_flag == "data")
    {
        QRegExp rx("^(020010).*$");
        if(rx.exactMatch(result))
            return Q_EMIT dataChanged(result.mid(6,32).toUtf8());
    }

    if(m_flag == "login")
    {
        QRegExp rx("^(0200014C).*$");
        bool isMatchLogin = rx.exactMatch(result.toUpper());

        if(!isMatchLogin)
            return Q_EMIT errRead();

        if(isMatchLogin)
            return emit successLoginChanged();
    }
}

Mifare::~Mifare(){
    if (_serial->isOpen()){
        _serial->close();
    }
}
