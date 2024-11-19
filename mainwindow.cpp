#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "services.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialisation du tableau (ServiceTableView_2) si nécessaire
    Services service;
    ui->serviceTableView_2->setModel(service.afficher());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Ajouter un service
void MainWindow::on_ajouterButton_clicked()
{
    int id = ui->idEdit->text().toInt();
    QString nom = ui->nomEdit->text();
    QString categorie = ui->categorieComboBox->currentText();
    double prix = ui->prixEdit->text().toDouble();
    QString description = ui->descriptionEdit->text();

    // Créer un objet service
    Services service(id, nom, categorie, prix, description);

    // Ajouter le service
    if (service.ajouter()) {
        QMessageBox::information(this, "Succès", "Service ajouté avec succès.");
        ui->serviceTableView_2->setModel(service.afficher());  // Mettre à jour le tableau
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du service.");
    }
}

// Supprimer un service
// Supprimer un service
void MainWindow::on_supprimerButton_clicked()
{
    // Récupérer l'ID du service à partir du champ de texte
    int id = ui->idEditSupprimer->text().toInt(); // Vérifie le nom de l'edit ici

    // Créer un objet Services et tenter de supprimer le service
    Services service;
    if (service.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Service supprimé avec succès.");
        ui->serviceTableView_2->setModel(service.afficher());  // Mettre à jour le tableau
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression du service.");
    }
}


// Modifier un service
void MainWindow::on_modifierButton_clicked()
{
    int id = ui->idEdit_modifier->text().toInt();
    QString nom = ui->nomEdit_modifier->text();
    QString categorie = ui->categorieComboBox_modifier->currentText(); // Vérifie le nom du comboBox ici
    double prix = ui->prixEdit_modifier->text().toDouble();
    QString description = ui->descriptionEdit_modifier->text();

    Services service(id, nom, categorie, prix, description);

    if (service.mettreAJour()) {
        QMessageBox::information(this, "Succès", "Service modifié avec succès.");
        ui->serviceTableView_2->setModel(service.afficher());  // Mettre à jour le tableau
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification du service.");
    }
}


