#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "services.h"
#include "arduino.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QtCharts>
#include <QDebug>
#include <QTcpSocket>
#include <QTextStream>
#include <QImage>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QSqlError>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    arduino(new Arduino(this))  // Initialisation correcte de l'objet Arduino
{
    ui->setupUi(this);

    // Connexion du signal envoyé par Arduino au slot handleArduinoSignal
    connect(arduino, &Arduino::boutonPresse, this, &MainWindow::handleArduinoSignal);


    // Initialisation de la connexion Arduino
    arduino->initialiserConnection();

    // Connexion du bouton 'exporterButton' au slot 'exportDataToPDF'
    connect(ui->exporterButton, &QPushButton::clicked, this, &MainWindow::exportDataToPDF);

    // Vérifiez si le bouton "Statistiques" existe déjà
    if (!statistiquesButton) {
        statistiquesButton = new QPushButton("Statistiques", this);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(statistiquesButton);
        ui->centralWidget->setLayout(layout);  // Assurez-vous que le layout est bien ajouté
        connect(statistiquesButton, &QPushButton::clicked, this, &MainWindow::on_statistiquesButton_clicked);
    }

    // Création du bouton supplémentaire et placement dans la fenêtre
    QPushButton *button = new QPushButton("Button 1", this);
    button->setGeometry(50, 50, 100, 30);  // Position et taille du bouton
    connect(button, &QPushButton::clicked, this, [&]() {
        qDebug() << "Button 1 Pressed";
        // Vous pouvez insérer ici la logique de mise à jour de la quantité
    });

    // Affichage des services dans la table view
    ui->serviceTableView->setModel(service.afficher());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete arduino;
}
void MainWindow::on_ajouterButton_clicked()
{
    int id = ui->idEdit->text().toInt();  // Récupère l'ID depuis l'interface utilisateur
    QString nom = ui->nomEdit->text();
    QString categorie = ui->categorieComboBox->currentText();
    double prix = ui->prixEdit->text().toDouble();
    QString description = ui->descriptionEdit->text();

    if (id == 0 || nom.isEmpty() || categorie.isEmpty() || prix <= 0 || description.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement !");
        return;
    }

    Services newService(id, nom, categorie, prix, description);
    if (newService.ajouter()) {
        QMessageBox::information(this, "Succès", "Service ajouté avec succès !");
        ui->serviceTableView->setModel(service.afficher());
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout : " + newService.ajouter());
    }
}

void MainWindow::on_supprimerButton_clicked()
{
    int id = ui->idEdit->text().toInt();
    if (id == 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide !");
        return;
    }

    if (service.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Service supprimé avec succès !");
        ui->serviceTableView->setModel(service.afficher());
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression !");
    }
}

void MainWindow::on_modifierButton_clicked()
{
    int id = ui->idEdit->text().toInt();
    QString nom = ui->nomEdit->text();
    QString categorie = ui->categorieComboBox->currentText();
    double prix = ui->prixEdit->text().toDouble();
    QString description = ui->descriptionEdit->text();

    if (id == 0 || nom.isEmpty() || categorie.isEmpty() || prix <= 0 || description.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement !");
        return;
    }

    // Vérifier si le service avec cet ID existe déjà dans la base
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM services WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec() || !query.next() || query.value(0).toInt() == 0) {
        QMessageBox::critical(this, "Erreur", "Aucun service trouvé avec cet ID !");
        return;
    }

    // Si l'ID est valide, effectuer la mise à jour
    Services updatedService(id, nom, categorie, prix, description);
    if (updatedService.mettreAJour()) {
        QMessageBox::information(this, "Succès", "Service modifié avec succès !");
        ui->serviceTableView->setModel(service.afficher());
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification !");
    }
}

void MainWindow::on_afficherButton_clicked()
{
    ui->serviceTableView->setModel(service.afficher());
    QMessageBox::information(this, "Affichage", "Liste des services mise à jour !");
}

void MainWindow::on_trierButton_clicked()
{
    ui->serviceTableView->setModel(service.trierParPrix());
}

void MainWindow::on_rechercheButton_clicked()
{
    QString nom = ui->rechercheEdit->text();
    ui->serviceTableView->setModel(service.rechercherParNom(nom));
}

void MainWindow::on_statistiquesButton_clicked()
{
    // Créer le graphique des statistiques
    QtCharts::QChartView *chartView = Services::genererGraphiqueStatistiques();

    // Ajouter le graphique à un widget
    QWidget *statistiquesWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(statistiquesWidget);
    layout->addWidget(chartView);

    // Afficher le widget des statistiques
    statistiquesWidget->setLayout(layout);
    statistiquesWidget->show();
}

void MainWindow::exportDataToPDF() {
    // Ouvrir une boîte de dialogue pour choisir l'emplacement du fichier PDF
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer en tant que PDF", "", "*.pdf");

    if (!filePath.isEmpty()) {
        // Si l'extension n'est pas .pdf, l'ajouter
        if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
            filePath.append(".pdf");
        }

        // Appeler la méthode exportToPDF() de l'objet service pour exporter les données
        if (service.exportToPDF(filePath)) {
            QMessageBox::information(this, "Export PDF", "Données exportées avec succès en PDF.");
        } else {
            QMessageBox::critical(this, "Erreur d'export PDF", "Échec de l'exportation des données en PDF.");
        }
    } else {
        // Si l'utilisateur annule, afficher une info
        QMessageBox::information(this, "Annulation", "Exportation annulée.");
    }

}
void MainWindow::on_exporterButton_clicked()
{
    // Cette méthode est appelée lorsque l'utilisateur clique sur le bouton exporterButton
    exportDataToPDF();  // Appeler la méthode d'exportation vers PDF
}
void MainWindow::on_remiseButton_clicked() {
    // Vérifier que le prix est saisi correctement
    bool okPrix;
    double prix = ui->prixEdit->text().toDouble(&okPrix);

    if (!okPrix || prix <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un prix valide (positif).");
        return;
    }

    // Vérifier que le pourcentage de remise est valide
    bool okRemise;
    double pourcentageRemise = ui->remiseLineEdit->text().toDouble(&okRemise);

    if (!okRemise || pourcentageRemise < 0 || pourcentageRemise > 100) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un pourcentage de remise valide (entre 0 et 100).");
        return;
    }

    // Calculer le nouveau prix après remise
    double remise = (prix * pourcentageRemise) / 100.0;
    double nouveauPrix = prix - remise;

    // Afficher le nouveau prix dans le QPlainTextEdit
    ui->prixApresRemiseLabel->setPlainText("Prix après remise : " + QString::number(nouveauPrix, 'f', 2) + " TND");
}
void MainWindow::on_convertirButton_clicked() {
    // Vérifier que le prix est saisi correctement
    bool ok;
    double prix = ui->prixEdit->text().toDouble(&ok);

    if (!ok || prix <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un prix valide (positif).");
        return;
    }

    // Récupérer la devise sélectionnée
    QString devise = ui->deviseComboBox->currentText();

    // Définir les taux de conversion
    QMap<QString, double> tauxDeConversion;
    tauxDeConversion["USD"] = 0.30; // Exemple : 1 TND = 0.30 USD
    tauxDeConversion["EUR"] = 0.27; // Exemple : 1 TND = 0.27 EUR
    tauxDeConversion["GBP"] = 0.23; // Exemple : 1 TND = 0.23 GBP
    tauxDeConversion["JPY"] = 33.00; // Exemple : 1 TND = 33.00 JPY

    // Calculer le prix converti
    if (!tauxDeConversion.contains(devise)) {
        QMessageBox::warning(this, "Erreur", "Devise non prise en charge.");
        return;
    }

    double prixConverti = prix * tauxDeConversion[devise];

    // Afficher le prix converti
    ui->prixConvertiLabel->setPlainText("Prix converti en " + devise + " : " + QString::number(prixConverti, 'f', 2));
}
void MainWindow::handleArduinoSignal(int boutonId) {
    // Mettre à jour la quantité du produit à 0 selon le bouton pressé
    int produitId = (boutonId == 1) ? 1 : 2;  // Exemple pour 1 produit lié au bouton 1 et un autre au bouton 2
    updateProductQuantity(produitId);
}

void MainWindow::updateProductQuantity(int produitId)
{
    // Déclaration des variables
    int newQuantity = 0;    // La nouvelle quantité que vous souhaitez mettre à jour
    int productId = 1;      // L'ID du produit à mettre à jour (remplacez-le par la valeur appropriée)

    // Préparez la requête SQL
    QSqlQuery query;
    query.prepare("UPDATE produits SET QUANTITE_EN_STOCK = :quantity WHERE ID = :id");

    // Liez les variables à la requête
    query.bindValue(":quantity", newQuantity);  // La nouvelle quantité
    query.bindValue(":id", productId);          // L'ID du produit à mettre à jour

    // Exécutez la requête
    if (query.exec()) {
        qDebug() << "Quantité mise à jour avec succès";
    } else {
        qDebug() << "Erreur lors de la mise à jour de la quantité: " << query.lastError().text();
    }
}
