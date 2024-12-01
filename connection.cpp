#include "connection.h"
#include <QDebug>

Connection::Connection()
{
}

bool Connection::createconnect(){
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("CPP_Project");
    db.setUserName("souha");
    db.setPassword("oracle");

    // Vérifiez la connexion ici et loggez si la connexion échoue
    if (!db.open()) {
        qDebug() << "Erreur de connexion : " << db.lastError().text();
        return false;
    }
    return true;
}

void Connection::closeConnection(){
    db.close();
}











