#include "arduino.h"

// Constructeur
Arduino::Arduino()
{
    dataBuffer = "";
    arduinoPortName = "";
    arduinoIsAvailable = false;
    serial = new QSerialPort;
}

// Obtenir le nom du port
QString Arduino::getPortName()
{
    return arduinoPortName;
}

// Obtenir l'objet QSerialPort
QSerialPort* Arduino::getSerial()
{
    return serial;
}

// Connecter au port Arduino
int Arduino::connectToArduino()
{
    // Parcourir tous les ports disponibles
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
            return 0; // Succès
        }
        return 1; // Échec d'ouverture du port
    }
    return -1; // Arduino non trouvé
}

// Déconnecter du port Arduino
int Arduino::disconnectFromArduino()
{
    if (serial->isOpen()) {
        serial->close();
        return 0; // Déconnexion réussie
    }
    return 1; // Échec de la déconnexion
}

// Envoyer des données à l'Arduino
int Arduino::sendDataToArduino(QByteArray data)
{
    if (serial->isWritable()) {
        serial->write(data); // Envoyer les données
        return 0; // Succès
    } else {
        qDebug() << "Impossible d'envoyer les données à l'Arduino !";
        return 1; // Échec
    }
}

// Recevoir des données de l'Arduino
QByteArray Arduino::receiveDataFromArduino()
{
    if (serial->isReadable()) {
        dataBuffer = serial->readAll(); // Lire les données disponibles
        return dataBuffer;
    }
    return ""; // Aucune donnée reçue
}
