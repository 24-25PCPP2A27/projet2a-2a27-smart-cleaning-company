#ifndef PRODUIT_H
#define PRODUIT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>
#include <QMap>

class Produit {
private:
    int id_produit;
    int prix;
    int quantite;  // Changed to avoid character confusion
    QDate date;    // Assuming date should be a QDate
    QString nom;
    QString categorie;


public:
    // Default constructor
    Produit() {}

    // Parameterized constructor
    Produit(int id, int p, int q, QDate d, QString n, QString c);


    // Getters
    int getIdProduit() const { return id_produit; }
    int getPrix() const { return prix; }
    int getQuantite() const { return quantite; }
    QDate getDate() const { return date; }
    QString getNom() const { return nom; }
    QString getCategorie() const { return categorie; }

    // Setters
    void setIdProduit(int id) { id_produit = id; }
    void setPrix(int p) { prix = p; }
    void setQuantite(int q) { quantite = q; }
    void setDate(QDate d) { date = d; }
    void setNom(QString n) { nom = n; }
    void setCategorie(QString c) { categorie = c; }

    // Database operations
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool modifier(int id);


    // Check if product exists in the database
    bool doesProductExist(int id_produit);

    // Search for a product by its ID
    QSqlQueryModel* recherche(int id_produit); // Added this function declaration

    QMap<QString, int> getCategorieStats();




};

#endif // PRODUIT_H

