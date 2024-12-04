#ifndef EMPLOYE_H
#define EMPLOYE_H
#include<QString>
#include<QSqlQuery>
#include<QSqlQueryModel>
#include <QDate>
#include <QMap>
#include "arduino.h"  // Ajoutez cette ligne pour inclure la classe Arduino


class employe
{
    QString nom , prenom, poste;
    int id;
    int salaire;
    QDate date_embauche;
     Arduino* arduino;
     static int compteurAjouts; // Compteur global pour gérer le buzzer
public:
    employe(){};
    employe(int id,QString nom,QString prenom ,QString poste ,int salaire ,QDate date_embauche);

    //getteres mteena
    QString getNom(){return nom;}
    QString getPrenom(){return prenom;}
    QString getPoste(){return poste;}
    int getID(){return id;}
    int getSalaire(){return salaire;}
    QDate getdate(){return date_embauche;}
      QMap<QString, int> statistiquesParSalaire();

    //setters mteena

    void setNom(QString n){nom=n;}
    void setPrenom(QString pre){prenom=pre;}
    void setPoste(QString po){poste=po;}
    void setID(int id ){this->id=id;}
    void setDateEmbauche(QDate date) { date_embauche = date; }
       // fonctionnalités
    bool ajouter();
    QSqlQueryModel * afficher();
    bool supprimer(int);
    bool update(int id, QString nom, QString prenom, QString poste, int salaire, QDate date_embauche);

    // Nouvelles fonctionnalités
       QMap<QString, int> statistiquesParPoste();
       QSqlQueryModel* rechercher(QString searchKey);
       QSqlQueryModel* trierParSalaire();
       bool ajouterPointage(int id, const QString& debutJournee, const QString& finJournee);
           QVector<QVector<QString>> getPointage() const;
           bool idExiste(int id); // Vérifie si un ID existe dans la base de données
           QVector<QVector<QString>> getPointageTable() const; // Accès à pointageTable
               QVector<QVector<QString>>& getPointageTableRef();         // Getters et setters

        void verifierEtActiverBuzzer(); // Méthode pour gérer le buzzer


        employe(Arduino* arduinoInstance): arduino(arduinoInstance){}
        QVector<QVector<QString>> pointageTable; // Tableau des pointages
          bool exporterPointagePDF(const QString& cheminFichier);
          QVector<QVector<QString>> getPointageData(); // Add this method to your class
     void on_pushButton_enregistrer_clicked();
};

#endif // EMPLOYE_H
