#include "mainwindoww3.h"
#include "ui_mainwindoww3.h"
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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QUrl>
#include "ui_mainwindoww3.h"
#include "mainwindoww3.h"
#include "mainwindow2.h"  // Inclure la classe pour mainwindow2
#include "mainwindoww3.h" // Inclure la classe pour mainwindoww3
#include "lacommande.h"




MainWindoww3::MainWindoww3(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindoww3)
{
    ui->setupUi(this); // Initialisation de l'interface

    // Vérification de l'existence du bouton "Statistiques" et connexion si nécessaire
    if (!ui->statistiquesButton) {
        // Si le bouton n'existe pas dans l'UI, on le crée dynamiquement
        statistiquesButton = new QPushButton("Statistiques", this);
        connect(statistiquesButton, &QPushButton::clicked, this, &MainWindoww3::on_statistiquesButton_clicked);

        // Vérifier si un layout existe pour ajouter le bouton
        QVBoxLayout *parentLayout = qobject_cast<QVBoxLayout *>(this->centralWidget()->layout());

        if (parentLayout) {
            parentLayout->addWidget(statistiquesButton);
        } else {
            // Créer un layout si aucun layout n'existe
            QVBoxLayout *newLayout = new QVBoxLayout();
            newLayout->addWidget(statistiquesButton);
            this->centralWidget()->setLayout(newLayout);
        }
    } else {
        // Si le bouton existe dans l'UI, on fait la connexion directement via `ui`
        connect(ui->statistiquesButton, &QPushButton::clicked, this, &MainWindoww3::on_statistiquesButton_clicked);
    }

    // Connexion pour le bouton "scheduleServiceButton"
    connect(ui->scheduleServiceButton, &QPushButton::clicked, this, &MainWindoww3::scheduleService);
    connect(ui->exporterButton, &QPushButton::clicked, this, &MainWindoww3::exportDataToPDF);

    // Affichage des services dans la table view
    ui->serviceTableView->setModel(service.afficher());
}


MainWindoww3::~MainWindoww3()
{
    delete ui;
}

void MainWindoww3::on_ajouterButton_clicked()
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

void MainWindoww3::on_supprimerButton_clicked()
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

void MainWindoww3::on_modifierButton_clicked()
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
void MainWindoww3::on_afficherButton_clicked()
{
    ui->serviceTableView->setModel(service.afficher());
    QMessageBox::information(this, "Affichage", "Liste des services mise à jour !");
}

void MainWindoww3::on_trierButton_clicked()
{
    ui->serviceTableView->setModel(service.trierParPrix());
}

void MainWindoww3::on_rechercheButton_clicked()
{
    QString nom = ui->rechercheEdit->text();
    ui->serviceTableView->setModel(service.rechercherParNom(nom));
}

void MainWindoww3::on_statistiquesButton_clicked()
{
    static QMap<QString, int> lastStats;

    // Générer les statistiques actuelles
    QMap<QString, int> currentStats;
    QSqlQuery query;
    QString sql = "SELECT CATEGORIE, COUNT(*) AS count FROM Services GROUP BY CATEGORIE";

    if (!query.exec(sql)) {
        qWarning() << "Error executing query:" << query.lastError().text();
        return;
    }

    // Vérifier que des résultats sont renvoyés
    if (!query.first()) {
        qWarning() << "No data found for the statistics query.";
        return; // Si aucun résultat, sortir de la fonction
    }

    // Remplir les statistiques
    do {
        QString category = query.value("CATEGORIE").toString();
        int count = query.value("count").toInt();
        currentStats[category] = count;
    } while (query.next());

    // Débogage : afficher les statistiques récupérées
    qDebug() << "Current stats:";
    for (auto it = currentStats.begin(); it != currentStats.end(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }

    // Si les statistiques n'ont pas changé, ne pas rafraîchir
    if (currentStats == lastStats) {
        qDebug() << "No change in statistics. Skipping refresh.";
        return; // Si les statistiques sont les mêmes, ne rien faire
    }

    // Mettre à jour les dernières statistiques
    lastStats = currentStats;

    // Supprimer les anciens widgets du layout du widget 'statistiquesWidget'
    QLayout *layout = ui->statistiquesWidget->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }

    // Créer un graphique avec les nouvelles statistiques
    QtCharts::QPieSeries *series = new QtCharts::QPieSeries();
    for (auto it = currentStats.begin(); it != currentStats.end(); ++it) {
        series->append(it.key(), it.value());
    }

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Services Statistics by CATEGORIE");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Ajouter le graphique à 'statistiquesWidget'
    QVBoxLayout *newLayout = new QVBoxLayout();
    newLayout->addWidget(chartView);
    ui->statistiquesWidget->setLayout(newLayout);

    qDebug() << "Statistics updated and chart added.";
}



void MainWindoww3::exportDataToPDF() {
    // Ouvrir une boîte de dialogue pour choisir l'emplacement du fichier PDF
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer en tant que PDF", "", "*.pdf");

    if (!filePath.isEmpty()) {
        // Si l'extension n'est pas .pdf, l'ajouter
        if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
            filePath.append(".pdf");
        }

        // Debug: Vérifier si la méthode est appelée
        qDebug() << "Appel de exportToPDF avec le chemin: " << filePath;

        // Appeler la méthode exportToPDF() de l'objet service pour exporter les données
        bool success = service.exportToPDF(filePath);
        if (success) {
            QMessageBox::information(this, "Export PDF", "Données exportées avec succès en PDF.");
        } else {
            QMessageBox::critical(this, "Erreur d'export PDF", "Échec de l'exportation des données en PDF.");
        }
    } else {
        // Si l'utilisateur annule, afficher une info
        QMessageBox::information(this, "Annulation", "Exportation annulée.");
    }
}


void MainWindoww3::on_exporterButton_clicked()
{
    // Cette méthode est appelée lorsque l'utilisateur clique sur le bouton exporterButton
    exportDataToPDF();  // Appeler la méthode d'exportation vers PDF
}

void MainWindoww3::on_remiseButton_clicked() {
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

void MainWindoww3::on_convertirButton_clicked() {
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
void MainWindoww3::on_serviceButton_clicked()
{
    if (!serviceWindow) { // Create the Service window only if it doesn't exist
        serviceWindow = new MainWindoww3(this);
    }
    serviceWindow->show(); // Show the Service window
}
void MainWindoww3::scheduleService() {
    // Récupère la date et l'heure sélectionnées
    QDateTime serviceTime = ui->serviceDateTime->dateTime();

    // Affiche un message de confirmation
    showConfirmationMessage("Service planifié pour : " + serviceTime.toString());
}

void MainWindoww3::showConfirmationMessage(const QString &message) {
    // Affiche le message dans la zone de texte prévue
    ui->confirmationMessage->setPlainText(message);
}
void MainWindoww3::on_pb_commande_clicked() {
    lacommande *com = new lacommande(this); // Create the second window
    com->show();                             // Show the second window
    this->hide();                                // Hide the current window (optional)
}


