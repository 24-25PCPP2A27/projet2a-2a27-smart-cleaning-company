#include "produit.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QMap>

Produit::Produit(int id_produit, int prix, int quantite, QDate date, QString nom, QString categorie)
{
    this->id_produit = id_produit;
    this->prix = prix;
    this->quantite = quantite;  // Corrected to match the updated variable name
    this->date = date;           // Now using QDate
    this->nom = nom;
    this->categorie = categorie;
}

bool Produit::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO produit (id_produit, nom, prix, quantite, categorie, datee) "
                  "VALUES (:id_produit, :nom, :prix, :quantite, :categorie, :date)");

    // Bind values
    query.bindValue(":id_produit", id_produit);
    query.bindValue(":prix", prix);
    query.bindValue(":quantite", quantite);  // Updated to use quantite
    query.bindValue(":date", date);           // QDate bound directly
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error adding product:" << query.lastError().text();
        return false;
    }
    return true;
}


QSqlQueryModel* Produit::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    // Debugging: List all available tables to check if PRODUIT is available
    QSqlQuery listTablesQuery;
    listTablesQuery.exec("SELECT table_name FROM all_tables");

    while (listTablesQuery.next()) {
        qDebug() << "Available table: " << listTablesQuery.value(0).toString();
    }

    // Prepare and execute the query to fetch all products
    query.prepare("SELECT * FROM PRODUIT");
    if (!query.exec()) {
        qDebug() << "Error executing SELECT query: " << query.lastError().text();
        return nullptr; // Handle query failure
    }

    model->setQuery(query);
    if (model->lastError().isValid()) {
        qDebug() << "Error setting query to model: " << model->lastError().text();
    }

    return model;
}


bool Produit::supprimer(int id_produit)
{
    QSqlQuery query;
    query.prepare("DELETE FROM produit WHERE id_produit = :id_produit");

    // Bind value
    query.bindValue(":id_produit", id_produit);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error deleting product:" << query.lastError().text();
        return false;
    }
    return true;
}

// Produit.cpp

#include "produit.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool Produit::doesProductExist(int id_produit)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM produit WHERE id_produit = :id_produit");
    query.bindValue(":id_produit", id_produit);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error checking product existence:" << query.lastError().text();
        return false;
    }

    // Check if the query returned a count greater than 0, meaning the product exists
    query.next();
    int count = query.value(0).toInt();
    return count > 0;  // Return true if the product exists
}

bool Produit::modifier(int id_produit)
{
    QSqlQuery query;
    query.prepare("UPDATE produit SET prix = :prix, quantite = :quantite, DATEE = :date, nom = :nom, categorie = :categorie WHERE id_produit = :id_produit");

    // Bind values
    query.bindValue(":prix", prix);
    query.bindValue(":quantite", quantite);
    query.bindValue(":date", date); // Corrected to bind the correct column 'DATEE'
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);
    query.bindValue(":id_produit", id_produit); // Bind the id for the WHERE clause

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error modifying product:" << query.lastError().text();
        return false;
    }
    return true;
}
// Produit class: Recherche function to find product by ID
QSqlQueryModel* Produit::recherche(int id_produit) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    // Prepare the SQL query to search for the product by ID
    query.prepare("SELECT * FROM produit WHERE id_produit = :id_produit");
    query.bindValue(":id_produit", id_produit);

    // Execute the query
    if (query.exec()) {
        model->setQuery(query);
    } else {
        qDebug() << "Error searching for product:" << query.lastError().text();
    }

    return model;
}

QMap<QString, int> Produit::getCategorieStats() {
    QMap<QString, int> stats;
    QSqlQuery query;

    // Query to get the count of products grouped by category
    query.prepare("SELECT categorie, COUNT(*) as count FROM produit GROUP BY categorie");

    if (!query.exec()) {
        qDebug() << "Error retrieving category statistics:" << query.lastError().text();
    } else {
        while (query.next()) {
            QString categorie = query.value("categorie").toString();
            int count = query.value("count").toInt();
            stats[categorie] = count;  // Store category and count in the map
        }
    }

    return stats;
}








