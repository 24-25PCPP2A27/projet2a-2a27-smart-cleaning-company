#ifndef ARDUINOO_H
#define ARDUINOO_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

class Arduinoo
{
public:
    Arduinoo();                          // Constructeur
    int connectToArduino();             // Connecter le PC à l'Arduino
    int disconnectFromArduino();        // Déconnecter
    int sendDataToArduino(QByteArray);  // Envoyer des données à l'Arduino
    QByteArray receiveDataFromArduino();// Recevoir des données de l'Arduino
    QString getPortName();              // Obtenir le nom du port de l'Arduino
    QSerialPort* getSerial();           // Accès au port série

private:
    QSerialPort *serial;                // Objet port série
    QString arduinoPortName;            // Nom du port de l'Arduino
    bool arduinoIsAvailable;            // Statut de disponibilité de l'Arduino
    QByteArray dataBuffer;              // Buffer de réception

    static const quint16 vendorID = 9025;  // ID du fournisseur pour Arduino Uno
    static const quint16 productID = 67;   // ID du produit pour Arduino Uno
};

#endif // ARDUINOO_H
