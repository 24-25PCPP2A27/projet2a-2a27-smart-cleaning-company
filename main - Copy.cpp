#include "mainwindow.h"
#include <QApplication>
#include "connection.h"
#include "arduino.h"
#include "employe.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    connection c;
    bool test = c.createconnection();
    MainWindow w;

    // Initialiser Arduino
    Arduino arduino;
    if (arduino.connect_arduino() == 0) {
        qDebug() << "Arduino connecté avec succès !";
    } else {
        qDebug() << "Échec de la connexion Arduino.";
    }

    if (test) {
        w.show();
        QMessageBox::information(nullptr,
                                 QObject::tr("Database Connection"),
                                 QObject::tr("Connection successful.\n Click Cancel to exit."),
                                 QMessageBox::Cancel);
    } else {
        QMessageBox::critical(nullptr,
                              QObject::tr("Database Connection"),
                              QObject::tr("Connection failed.\n Click Cancel to exit."),
                              QMessageBox::Cancel);
    }

    return a.exec();
}
