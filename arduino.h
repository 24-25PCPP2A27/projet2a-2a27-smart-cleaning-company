#ifndef ARDUINO_H
#define ARDUINO_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

class Arduino
{
public:     // Methods
    Arduino();
    int connect_arduino();
    int close_arduino();
    void write_to_arduino(QByteArray);
    QByteArray read_from_arduino();
    QSerialPort* getserial();
    QString getarduino_port_name();

    // Buzzer-related methods
    void increment_add_counter();
    int get_add_counter() const; // Get current counter value
    void reset_add_counter(); // Reset the counter
    void check_and_activate_buzzer(); // Activate buzzer if limit exceeded

private:
    QSerialPort *serial;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_producy_id = 67;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray data;

    // Buzzer-related variables
    int add_counter;
    const int max_adds = 2;
};

#endif // ARDUINO_H
