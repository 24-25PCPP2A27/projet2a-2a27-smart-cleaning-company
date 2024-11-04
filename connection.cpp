#include "connection.h"

Connection::Connection(){}

bool Connection::createconnection(){
    db = QSqlDatabase::addDatabase("QODBC");
    bool test=false;
    db.setDatabaseName("");
    db.setUserName("");
    db.setPassword("");
    if (db.open()) test=true;
    return test;
}
void Connection::closeConnection(){db.close();}
