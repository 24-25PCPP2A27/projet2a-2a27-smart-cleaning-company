#include "connection.h"

connection::connection(){}

bool connection::createconnection(){
    bool test=false;
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("CPP_Project");
    db.setUserName("souha");
    db.setPassword("oracle");
    if (db.open()) test=true;
    return test;
}

