#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class SerialCommunication : public QObject {
    Q_OBJECT  // Nécessaire pour les signaux/slots Qt

public:
    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication();

    void connectToArduino();
    void disconnectFromArduino();
    void checkProductAndActivateVibrator();

signals:
    void buttonPressed(int boutonId);

private slots:
    void readData();

private:
    QSerialPort serialPort;
};

#endif // SERIALCOMMUNICATION_H


