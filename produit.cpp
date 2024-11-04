#include "produit.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

Produit::Produit(int id_produit, int prix, int quantité, int date, QString nom, QString categorie)
{
    this->id_produit = id_produit;
    this->prix = prix;
    this->quantité = quantité;
    this->date = date;
    this->nom = nom;
    this->categorie = categorie;
}

bool Produit::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO produits (id_produit, prix, quantité, date, nom, categorie) "
                  "VALUES (:id_produit, :prix, :quantité, :date, :nom, :categorie)");

    // Bind values
    query.bindValue(":id_produit", id_produit);
    query.bindValue(":prix", prix);
    query.bindValue(":quantité", quantité);
    query.bindValue(":date", date);
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error adding product:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel * Produit::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM produits");
    query.exec();

    model->setQuery(query);
    return model;
}

bool Produit::supprimer(int id_produit)
{
    QSqlQuery query;
    query.prepare("DELETE FROM produits WHERE id_produit = :id_produit");

    // Bind value
    query.bindValue(":id_produit", id_produit);

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Error deleting product:" << query.lastError().text();
        return false;
    }
    return true;
}
bool Produit::modifier(int id_produit)
{
    QSqlQuery query;
    query.prepare("UPDATE produits SET prix = :prix, quantité = :quantité, date = :date, nom = :nom, categorie = :categorie WHERE id_produit = :id_produit");

    // Bind values
    query.bindValue(":prix", prix);
    query.bindValue(":quantité", quantité);
    query.bindValue(":date", date);
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
