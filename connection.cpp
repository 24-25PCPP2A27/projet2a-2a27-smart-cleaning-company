#include "connection.h"

connection::connection(){}

bool connection::createconnection(){
    bool test=false;
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Ateliers_CRUD");
    db.setUserName("crud");
    db.setPassword("oracle");
    if (db.open()) test=true;
    return test;
}

