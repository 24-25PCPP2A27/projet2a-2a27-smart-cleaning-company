#include "connection.h"
#include <QDebug>

Connection::Connection()
{
}
bool Connection::createconnect() {
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QODBC");
        db.setDatabaseName("CPP_Project");
        db.setUserName("souha");
        db.setPassword("oracle");
    }

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Erreur de connexion :" << db.lastError().text();
        return false;
    }

    return true;
}











