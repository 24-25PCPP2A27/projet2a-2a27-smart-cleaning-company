#include "serialcommunication.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent) : QObject(parent) {
    // Connecte le signal readyRead pour gérer les données entrantes

}

SerialCommunication::~SerialCommunication() {
    disconnectFromArduino();
}

void SerialCommunication::connectToArduino() {
    serialPort.setPortName("COM5");  // Remplacez par votre port correct
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to Arduino!";
    } else {
        qDebug() << "Failed to connect to Arduino: " << serialPort.errorString();
    }
}

void SerialCommunication::disconnectFromArduino() {
    if (serialPort.isOpen()) {
        serialPort.close();
        qDebug() << "Disconnected from Arduino";
    }
}

void SerialCommunication::checkProductAndActivateVibrator() {
    QSqlQuery query;
    query.prepare("SELECT id, quantity FROM PRODUIT WHERE quantity = 0");
    if (query.exec()) {
        while (query.next()) {
            int productId = query.value(0).toInt();

            // Si le port série est ouvert, envoyer un signal à l'Arduino
            if (serialPort.isOpen()) {
                serialPort.write("1");  // Envoyer '1' pour activer le vibreur
                qDebug() << "Vibrator activated for product ID: " << productId;
            }
        }
    } else {
        qDebug() << "Error checking products: " << query.lastError().text();
    }
}

void SerialCommunication::readData() {
    QByteArray data = serialPort.readAll();
    QString message = QString::fromUtf8(data).trimmed();

    qDebug() << "Data received from Arduino:" << message;

    // Exemple de traitement des messages
    if (message.contains("Button1")) {
        emit buttonPressed(1);
    } else if (message.contains("Button2")) {
        emit buttonPressed(2);
    }
}


