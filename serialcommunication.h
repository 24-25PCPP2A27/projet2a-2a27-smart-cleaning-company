#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QtSerialPort/QSerialPort>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class SerialCommunication {
public:
    SerialCommunication();
    ~SerialCommunication();

    void connectToArduino();  // Establish connection with Arduino
    void checkProductAndActivateVibrator();  // Check products and activate vibrator if needed
    void disconnectFromArduino();  // Disconnect from Arduino

private:
    QSerialPort serialPort;
};

#endif // SERIALCOMMUNICATION_H
