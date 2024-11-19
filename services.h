#ifndef SERVICES_H
#define SERVICES_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Services
{
private:
    int id;
    QString nom;
    QString categorie;
    double prix;
    QString description;

public:
    Services();
    Services(int id, QString nom, QString categorie, double prix, QString description);

    bool ajouter();
    bool supprimer(int id);
    bool mettreAJour();
    QSqlQueryModel* afficher();
};

#endif // SERVICES_H
