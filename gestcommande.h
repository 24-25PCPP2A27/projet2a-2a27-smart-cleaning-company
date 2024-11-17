#ifndef GESTCOMMANDE_H
#define GESTCOMMANDE_H
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class Commande
{
    int id;
    QString date;
    QString statut;
    QString modePaiement;

public:
    Commande();
    Commande(int id, QString date, QString statut, QString modePaiement);

    int getID() const { return id; }
    QString getDate() const { return date; }
    QString getStatut() const { return statut; }
    QString getModePaiement() const { return modePaiement; }

    void setID(int id) { this->id = id; }
    void setDate(const QString &d) { date = d; }
    void setStatut(const QString &s) { statut = s; }
    void setModePaiement(const QString &m) { modePaiement = m; }
    void genererRapportPDF(QString cheminFichier);



    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool modifier();
    QSqlQueryModel* afficherParID(bool ascending);
    QSqlQueryModel* rechercherParID(int id);
    QMap<QString, int> getStatisticsByStatus();





};


#endif // GESTCOMMANDE_H
