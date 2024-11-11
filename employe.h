#ifndef EMPLOYE_H
#define EMPLOYE_H
#include<QString>
#include<QSqlQuery>
#include<QSqlQueryModel>
#include <QDate>


class employe
{
    QString nom , prenom, poste;
    int id;
    int salaire;
    QDate date_embauche;

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


};

#endif // EMPLOYE_H
