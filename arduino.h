#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>

class Arduino : public QObject
{
    Q_OBJECT

public:
    explicit Arduino(QObject *parent = nullptr);
    void initialiserConnection();  // Initialise la connexion série avec Arduino

private:
    QSerialPort *port;

private slots:
    void readData();  // Lit les données envoyées par Arduino

signals:
    void boutonPresse(int);  // Signal émis lors de la pression d'un bouton (envoyé à Qt)
};

#endif // ARDUINO_H




