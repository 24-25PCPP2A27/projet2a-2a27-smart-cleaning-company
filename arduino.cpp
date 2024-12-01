#include "arduino.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

Arduino::Arduino(QObject *parent) : QObject(parent), port(new QSerialPort(this))
{
    // Connexion du signal de réception de données
    connect(port, &QSerialPort::readyRead, this, &Arduino::readData);  // Appelle readData() quand des données arrivent
}

void Arduino::initialiserConnection()
{
    // Configure la connexion série avec Arduino
    port->setPortName("COM6");  // Vérifiez que le nom du port COM est correct (peut être COMx sur Windows)
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (port->open(QIODevice::ReadWrite)) {
        qDebug() << "Connexion avec Arduino réussie!";
    } else {
        qDebug() << "Erreur de connexion avec Arduino: " << port->errorString();
    }
}

void Arduino::readData() {
    static QByteArray buffer;  // Tampon pour stocker les données reçues en plusieurs parties
    buffer.append(port->readAll());  // Ajoute les nouvelles données au tampon

    // Recherchez le caractère de fin de ligne '\n'
    int endIndex = buffer.indexOf('\n');
    while (endIndex != -1) {
        QByteArray completeMessage = buffer.left(endIndex);  // Message complet
        buffer.remove(0, endIndex + 1);  // Retire le message traité du tampon

        qDebug() << "Données reçues : " << completeMessage;  // Affiche le message complet

        // Vérifiez les messages spécifiques pour chaque bouton
        if (completeMessage.contains("Button 1 Pressed")) {
            qDebug() << "Le bouton 1 a été pressé !";
            emit boutonPresse(1);  // Émet le signal pour le bouton 1 pressé
        } else if (completeMessage.contains("Button 2 Pressed")) {
            qDebug() << "Le bouton 2 a été pressé !";
            emit boutonPresse(2);  // Émet le signal pour le bouton 2 pressé
        }

        // Recherchez le prochain message complet
        endIndex = buffer.indexOf('\n');
    }
}


