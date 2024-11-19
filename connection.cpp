#include "connection.h"

Connection::Connection()
{
}

bool Connection::createconnect(){
    db = QSqlDatabase::addDatabase("QODBC");
    bool test = false;
    db.setDatabaseName("CPP_Project");
    db.setUserName("souha");
    db.setPassword("oracle");
    if (db.open())
        test = true;
    return test;
}

void Connection::closeConnection(){
    db.close();
}











