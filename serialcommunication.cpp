#include "serialcommunication.h"

SerialCommunication::SerialCommunication() {
    // Constructor can be used for any initialization if needed
}

SerialCommunication::~SerialCommunication() {
    // Destructor to clean up resources
    disconnectFromArduino();
}

void SerialCommunication::connectToArduino() {
    serialPort.setPortName("COM5");  // Change to your correct COM port
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

void SerialCommunication::checkProductAndActivateVibrator() {
    QSqlQuery query;
    query.prepare("SELECT id, quantity FROM PRODUIT WHERE quantity = 0");
    if (query.exec()) {
        while (query.next()) {
            int productId = query.value(0).toInt();
            int quantity = query.value(1).toInt();

            if (quantity == 0) {
                // Send a signal to Arduino to activate the vibrator
                if (serialPort.isOpen()) {
                    serialPort.write("1");  // Send '1' to turn on the vibrator
                    qDebug() << "Vibrator activated for product ID: " << productId;
                }
            }
        }
    } else {
        qDebug() << "Error checking products: " << query.lastError().text();
    }
}

void SerialCommunication::disconnectFromArduino() {
    if (serialPort.isOpen()) {
        serialPort.close();
        qDebug() << "Disconnected from Arduino";
    }
}

