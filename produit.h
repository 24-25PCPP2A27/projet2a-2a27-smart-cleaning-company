#ifndef PRODUIT_H
#define PRODUIT_H
#include<QString>
#include<QSqlQuery>
#include<QSqlQueryModel>

class Produit
{
    QString nom, categorie;
    int id_produit, prix, quantité , date;
public:
    Produit(){}
    Produit(int, int, int, int, QString, QString);

    QString getNom(){return nom;}
    QString getcategorie(){return categorie;}
    int getID_PRODUIT(){return id_produit;}
    int getprix(){return prix;}
    int getquantité(){return quantité;}
    int getdate(){return date;}

    void setNom(QString n){nom=n;}
    void setcategorie(QString c){nom=c;}
    void setID_PRODUIT(int id_produit){this->id_produit=id_produit;}
    void setprix(int prix){this->prix=prix;}
    void setquantité(int quantité){this->quantité=quantité;}
    void setdate(int date){this->date=date;}

    bool ajouter();
    QSqlQueryModel * afficher();
    bool supprimer(int);
    bool modifier(int id_produit);




};

#endif // PRODUIT_H
