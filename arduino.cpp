#include "arduino.h"

Arduino::Arduino()
{
    data = "";
    arduino_port_name = "";
    arduino_is_available = false;
    serial = new QSerialPort;

    add_counter = 0; // Initialize the addition counter
}

QString Arduino::getarduino_port_name()
{
    return arduino_port_name;
}

QSerialPort* Arduino::getserial()
{
    return serial;
}

int Arduino::connect_arduino()
{
    foreach (const QSerialPortInfo &serial_port_info, QSerialPortInfo::availablePorts()) {
        if (serial_port_info.hasVendorIdentifier() && serial_port_info.hasProductIdentifier()) {
            if (serial_port_info.vendorIdentifier() == arduino_uno_vendor_id &&
                serial_port_info.productIdentifier() == arduino_uno_producy_id) {
                arduino_is_available = true;
                arduino_port_name = serial_port_info.portName();
            }
        }
    }

    qDebug() << "arduino_port_name is :" << arduino_port_name;

    if (arduino_is_available) {
        serial->setPortName(arduino_port_name);
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

int Arduino::close_arduino()
{
    if (serial->isOpen()) {
        serial->close();
        return 0;
    }
    return 1;
}

QByteArray Arduino::read_from_arduino()
{
    if (serial->isReadable()) {
        data = serial->readAll();
        return data;
    }
    return QByteArray();
}

void Arduino::write_to_arduino(QByteArray d)
{
    if (!serial->isOpen()) {
        qDebug() << "Serial port is closed. Attempting to reopen...";
        if (!serial->open(QSerialPort::ReadWrite)) {
            qDebug() << "Failed to reopen the serial port!";
            return;
        }
    }

    if (serial->isWritable()) {
        serial->write(d);
        qDebug() << "Command sent to Arduino:" << d;
    } else {
        qDebug() << "Serial port is not writable!";
    }
}



// Increment addition counter
void Arduino::increment_add_counter()
{
    add_counter++;
    qDebug() << "Addition counter incremented to:" << add_counter;
}

// Get the current counter value
int Arduino::get_add_counter() const
{
    return add_counter;
}

// Reset the counter to zero
void Arduino::reset_add_counter()
{
    add_counter = 0;
    qDebug() << "Addition counter reset.";
}

// Check and activate buzzer if the limit is exceeded
void Arduino::check_and_activate_buzzer()
{
    if (add_counter > max_adds) {
        QByteArray command = "BUZZER_ON"; // Command to activate buzzer
        write_to_arduino(command);
        qDebug() << "Buzzer activated!";
        reset_add_counter(); // Reset the counter after activating the buzzer
    }
}
