#include "connection.h"



Connection::Connection(){}



bool Connection::createconnection()

{

db = QSqlDatabase::addDatabase("QODBC");

bool test=false;

db.setDatabaseName("Ateliers_CRUD");

db.setUserName("crud");

db.setPassword("oracle");



if (db.open()) test=true;

    return  test;

}

void Connection::closeConnection(){db.close();}

