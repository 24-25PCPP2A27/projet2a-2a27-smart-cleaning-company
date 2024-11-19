#include "services.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QMessageBox>

Services::Services() {}

Services::Services(int id, QString nom, QString categorie, double prix, QString description)
{
    this->id = id;
    this->nom = nom;
    this->categorie = categorie;
    this->prix = prix;
    this->description = description;
}

// Ajouter un service
bool Services::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO services (id, nom, categorie, prix, description) "
                  "VALUES (:id, :nom, :categorie, :prix, :description)");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);
    query.bindValue(":prix", prix);
    query.bindValue(":description", description);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

// Supprimer un service
bool Services::supprimer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM services WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

// Mettre à jour un service
bool Services::mettreAJour()
{
    QSqlQuery query;
    query.prepare("UPDATE services SET nom = :nom, categorie = :categorie, prix = :prix, description = :description WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);
    query.bindValue(":prix", prix);
    query.bindValue(":description", description);

    if (query.exec()) {
        return true;
    } else {
        return false;
    }
}

// Afficher tous les services
QSqlQueryModel* Services::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query("SELECT * FROM services");

    model->setQuery(query);
    return model;
}

