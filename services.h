#ifndef SERVICES_H
#define SERVICES_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>
#include <QtCharts>

class Services
{
public:
    Services();
    Services(int id, QString nom, QString categorie, double prix, QString description);

    bool ajouter();
    bool supprimer(int id);
    bool mettreAJour();
    QSqlQueryModel* afficher();
    QSqlQueryModel* trierParPrix();
    QSqlQueryModel* rechercherParNom(const QString& nom);
    double appliquerRemise(double pourcentage);
    static QMap<QString, int> getStatistiquesParCategorie();
    static QtCharts::QChartView* genererGraphiqueStatistiques();
    bool exportToPDF(const QString &filePath);
    bool updateProductQuantity(int produitId, int nouvelleQuantite);



private:
    int id;
    QString nom;
    QString categorie;
    double prix;
    QString description;
};

#endif // SERVICES_H
