#include "arduinoo.h"


Arduinoo::Arduinoo()
{
    dataBuffer = "";
    arduinoPortName = "";
    arduinoIsAvailable = false;
    serial = new QSerialPort;
}


QString Arduinoo::getPortName()
{
    return arduinoPortName;
}


QSerialPort* Arduinoo::getSerial()
{
    return serial;
}


int Arduinoo::connectToArduino()
{

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {
            if (serialPortInfo.vendorIdentifier() == vendorID && serialPortInfo.productIdentifier() == productID) {
                arduinoIsAvailable = true;
                arduinoPortName = serialPortInfo.portName();
            }
        }
    }

    qDebug() << "Nom du port Arduino : " << arduinoPortName;

    if (arduinoIsAvailable) {
        serial->setPortName(arduinoPortName);
        if (serial->open(QSerialPort::ReadWrite)) {
            serial->setBaudRate(QSerialPort::Baud9600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            return 0;
        }
        return 1;
    }
    return -1;
}


int Arduinoo::disconnectFromArduino()
{
    if (serial->isOpen()) {
        serial->close();
        return 0;
    }
    return 1;
}


int Arduinoo::sendDataToArduino(QByteArray data)
{
    if (serial->isWritable()) {
        serial->write(data);
        return 0;
    } else {
        qDebug() << "Impossible d'envoyer les données à l'Arduino !";
        return 1;
    }
}


QByteArray Arduinoo::receiveDataFromArduino()
{
    if (serial->isReadable()) {
        dataBuffer = serial->readAll();
        return dataBuffer;
    }
    return "";
}
