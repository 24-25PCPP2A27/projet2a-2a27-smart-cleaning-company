#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "employe.h"
#include <QFileDialog>
#include <QSqlQueryModel>
#include <QFileDialog>   // Pour les boîtes de dialogue de fichier
#include <QPdfWriter>    // Pour écrire des PDF
#include <QPainter>      // Pour dessiner dans le PDF
#include "employe.h"
#include <QDebug>
#include<QSqlError>
#include <QStandardItemModel>
#include <QPalette>
#include <QColor>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QPieSeries>
#include <QSerialPort>    // Pour la communication série
#include <QSerialPortInfo> // Pour trouver les ports série disponibles

using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    ui->tableView->setModel(etmp.afficher());
    initialiserTableView();
    employeInstance = new employe();  // Creates a new instance of employe

     // Initialisation du modèle d'historique
         historiqueModel = new QStandardItemModel(this);
         historiqueModel->setHorizontalHeaderLabels({"Action", "Détails", "Date"});
         ui->tableView_historique->setModel(historiqueModel);
        connect(ui->pushButton_statistiques_salaire, &QPushButton::clicked, this, &MainWindow::on_pushButton_statistiques_salaire_clicked);
        connect(ui->pushButton_sauvegarder, &QPushButton::clicked, this, &MainWindow::on_pushButton_sauvegarder_clicked);
        connect(ui->pushButton_exporterPDF, &QPushButton::clicked, this, &MainWindow::on_pushButton_exporterPDF_clicked); // Connecter le bouton d'export PDF
        connect(ui->pushButton_enregistrer, &QPushButton::clicked, this, &MainWindow::on_pushButton_enregistrer_clicked);
        connect(ui->pushButton_employe, &QPushButton::clicked, this, &MainWindow::on_pushButton_employe_clicked);



}

MainWindow::~MainWindow()
{
    delete ui;
     delete employeInstance;

      }




// Ajout d'un employé
void MainWindow::on_pushButton_ajouter_clicked()
{
    if (ui->lineEdit_ID->text().isEmpty() || ui->lineEdit_nom->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    int id = ui->lineEdit_ID->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    int salaire = ui->lineEdit_salaire->text().toInt();
    QString poste = ui->lineEdit_poste->text();
    QDate date = QDate::fromString(ui->dateEdit_date->text(), "dd-MM-yyyy");

    employe e(id, nom, prenom, poste, salaire, date);

    if (e.ajouter()) {
        ui->tableView->setModel(etmp.afficher());
        QMessageBox::information(this, "Succès", "Employé ajouté avec succès!");

        // Mise à jour de l'historique
        updateHistorique("Ajout", "Ajout de l'employé " + nom + " " + prenom);

        // Faire buzzer l'Arduino

    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'employé.");
    }
}

// Suppression d'un employé
void MainWindow::on_pushButton_supprimer_clicked()
{
    int id = ui->lineEdit_ID->text().toInt();
    if (etmp.supprimer(id)) {
        ui->tableView->setModel(etmp.afficher());
        QMessageBox::information(this, "Succès", "Employé supprimé avec succès.");
        updateHistorique("Suppression", "Suppression de l'employé avec ID: " + QString::number(id));
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression de l'employé.");
    }
}

void MainWindow::on_pushButton_update_clicked() {
    int id = ui->lineEdit_ID->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    int salaire = ui->lineEdit_salaire->text().toInt();
    QString poste = ui->lineEdit_poste->text();
    QDate date = QDate::fromString(ui->dateEdit_date->text(), "dd-MM-yyyy");

    bool test = etmp.update(id, nom, prenom, poste, salaire, date);
    if (test) {
        QMessageBox::information(nullptr, QObject::tr("ok"),
                                 QObject::tr("update effectué\n" "click to exit."));
    } else {
        QMessageBox::critical(nullptr, QObject::tr("not ok"),
                              QObject::tr("update non effectué.\n" "click cancel to exit."),
                              QMessageBox::Cancel);
    }
}




void MainWindow::on_pushButton_rechercher_clicked() {
    QString searchKey = ui->lineEdit_recherche->text(); // Assure-toi que `lineEdit_recherche` est dans `mainwindow.ui`
    ui->tableView->setModel(etmp.rechercher(searchKey)); // Utilise la fonction rechercher
}



void MainWindow::on_pushButton_trier_clicked() {
    ui->tableView->setModel(etmp.trierParSalaire());
}


void MainWindow::on_pushButton_statistiques_clicked() {
    // Récupérer les statistiques par poste depuis la base de données
    QMap<QString, int> stats = etmp.statistiquesParPoste();

    if (stats.isEmpty()) {
        QMessageBox::warning(this, "Statistiques", "Aucune donnée trouvée pour les statistiques.");
        return;
    }

    // Création d'une scène pour le QGraphicsView
    QGraphicsScene *scene = new QGraphicsScene(this);

    // Définir les dimensions et espacement des carrés
    int squareSize = 50;
    int margin = 10;
    int x = 0, y = 0; // Position initiale

    // Génération des carrés pour chaque poste
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        // Couleur aléatoire pour chaque poste
        QColor color(qrand() % 256, qrand() % 256, qrand() % 256);

        // Créer un carré
        QRectF rect(x, y, squareSize, squareSize);
        QGraphicsRectItem *square = scene->addRect(rect, QPen(Qt::black), QBrush(color));

        // Ajouter un texte à côté du carré
        QString label = it.key() + " (" + QString::number(it.value()) + ")";
        QGraphicsTextItem *text = scene->addText(label);
        text->setPos(x, y + squareSize + 5);

        // Calculer la position pour le prochain carré
        x += squareSize + margin;
        if (x + squareSize > ui->graphicsView_statistiques->width()) {
            x = 0;
            y += squareSize + margin + 20; // Aller à la ligne suivante
        }
    }

    // Associer la scène au QGraphicsView
    ui->graphicsView_statistiques->setScene(scene);
}




void MainWindow::on_pushButton_exporterPDF_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if (!filePath.isEmpty()) {
        QPdfWriter pdfWriter(filePath);
        QPainter painter(&pdfWriter);
        int yPos = 100;

        QSqlQueryModel *model = etmp.afficher();

        // Loop through the rows of the model and export each row to the PDF
        for (int row = 0; row < model->rowCount(); ++row) {
            int xPos = 100;  // Start x-position for the first column

            // Loop through the columns for each row to add spacing
            for (int col = 0; col < model->columnCount(); ++col) {
                QString cellData = model->data(model->index(row, col)).toString();

                // Draw each column with 600 units of horizontal space between them
                painter.drawText(xPos, yPos, cellData);
                xPos += 900;  // Increase x-position for the next column
            }

            // Increase y-position for the next row with 300 units of vertical space
            yPos += 300;

            // If the content exceeds the page height, create a new page
            if (yPos > pdfWriter.height() - 50) {
                pdfWriter.newPage();
                yPos = 100; // Reset y-position for the new page
            }
        }

        // End the PDF drawing
        painter.end();

        QMessageBox::information(this, "Exporter PDF", "PDF exporté avec succès.");
    }
}







void MainWindow::updateHistorique(const QString &action, const QString &details) {
    // Ajouter une nouvelle action à l'historique
    HistoriqueAction historique;
    historique.action = action;
    historique.details = details;
    historique.date = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    historiqueList.append(historique);  // Ajouter à la liste des actions

    // Ajouter à l'affichage dans le tableView
    QStandardItem *actionItem = new QStandardItem(action);
    QStandardItem *detailsItem = new QStandardItem(details);
    QStandardItem *dateItem = new QStandardItem(historique.date);

    historiqueModel->appendRow({actionItem, detailsItem, dateItem});
}











void MainWindow::on_pushButton_statistiques_salaire_clicked() {
    QMap<QString, int> stats = etmp.statistiquesParSalaire();

    if (stats.isEmpty()) {
        QMessageBox::warning(this, "Statistiques", "Aucune donnée trouvée pour les statistiques par salaire.");
        return;
    }

    // Création de la série de diagramme circulaire
    QPieSeries *series = new QPieSeries();
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
    }

    // Personnaliser les tranches (facultatif)
    for (auto slice : series->slices()) {
        slice->setLabelVisible(true); // Rendre les labels visibles
        slice->setLabelFont(QFont("Arial", 9)); // Agrandir la taille des labels
        slice->setExploded(true); // Exploser légèrement les tranches
    }

    // Création du graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des employés par salaire");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Création de la vue graphique
    QGraphicsScene *scene = new QGraphicsScene(this);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Ajuster la taille du QChartView
    chartView->setFixedSize(641, 441);  // Taille personnalisée du graphique

    // Ajouter le QChartView à la scène
    scene->addWidget(chartView);

    // Associer la scène au QGraphicsView
    ui->graphicsView_salaire->setScene(scene);
}

void MainWindow::on_pushButton_sauvegarder_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Sauvegarder l'historique", "", "Fichiers Texte (*.txt)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // Écrire un en-tête
            out << "Action\tDétails\tDate\n";
            out << "------------------------------------------\n";

            // Parcourir les données de l'historique et les écrire
            for (const HistoriqueAction &action : historiqueList) {
                out << action.action << "\t" << action.details << "\t" << action.date << "\n";
            }

            file.close();
            QMessageBox::information(this, "Sauvegarde réussie", "L'historique a été sauvegardé avec succès dans " + filePath);
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier pour la sauvegarde.");
        }
    }
}


void MainWindow::initialiserTableView() {
    tableModel = new QStandardItemModel(this);
    tableModel->setColumnCount(3);
    tableModel->setHorizontalHeaderLabels({"ID", "Début Journée", "Fin Journée"});
    ui->tableView_pointage->setModel(tableModel);
}



void MainWindow::on_pushButton_commencer_clicked()
{
    if (ui->lineEdit_ID->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID.");
        return;
    }

    int id = ui->lineEdit_ID->text().toInt();
    QString debutJournee = QTime::currentTime().toString("HH:mm:ss");

    // Vérifier si l'ID existe dans la base de données
    if (!employeObj.idExiste(id)) {
        QMessageBox::critical(this, "Erreur", "L'ID n'existe pas dans la base de données des employés.");
        return;
    }

    if (!employeObj.ajouterPointage(id, debutJournee, "")) {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du pointage.");
    } else {
        afficherPointage();
        QMessageBox::information(this, "Pointage", "Début de journée enregistré pour l'ID " + QString::number(id));
    }
}



void MainWindow::on_pushButton_finJournee_clicked()
{
    if (ui->lineEdit_ID->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID.");
        return;
    }

    int id = ui->lineEdit_ID->text().toInt(); // Utiliser l'ID saisi
    QString finJournee = QTime::currentTime().toString("HH:mm:ss");

    // Vérifier si l'ID existe déjà dans le tableau des pointages
    bool updated = false;
    for (QVector<QString>& ligne : employeObj.pointageTable) { // Assurez-vous que `pointageTable` est accessible ici
        if (ligne[0] == QString::number(id)) {
            ligne[2] = finJournee; // Mettre à jour l'heure de fin
            updated = true;
            break;
        }
    }

    if (!updated) {
        QMessageBox::critical(this, "Erreur", "L'ID n'existe pas dans les pointages.");
    } else {
        afficherPointage();
        QMessageBox::information(this, "Pointage", "Fin de journée enregistrée pour l'ID " + QString::number(id));
    }
}




void MainWindow::afficherPointage()
{
    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"ID", "Début Journée", "Fin Journée"});

    QVector<QVector<QString>> pointage = employeObj.getPointage();
    for (const QVector<QString>& ligne : pointage) {
        int id = ligne[0].toInt();

        // Vérifier si l'ID existe dans la base de données
        if (!employeObj.idExiste(id)) {
            continue; // Ignorer les IDs non valides
        }

        QList<QStandardItem*> row;
        for (const QString& value : ligne) {
            row.append(new QStandardItem(value));
        }
        tableModel->appendRow(row);
    }
}


void MainWindow::on_pushButton_enregistrer_clicked() {
    // Let the user choose a file path for saving the PDF
    QString filePath = QFileDialog::getSaveFileName(nullptr, "Save PDF", "", "PDF Files (*.pdf)");

    if (!filePath.isEmpty()) {
        bool success = exporterPointagePDF(filePath);

        if (success) {
            qDebug() << "PDF export successful!";
        } else {
            qDebug() << "PDF export failed!";
        }
    }
}
bool MainWindow::exporterPointagePDF(const QString& filePath) {
    // Create a QPdfWriter instance to generate the PDF
    QPdfWriter writer(filePath);
    QPainter painter(&writer);

    if (!painter.isActive()) {
        qDebug() << "Failed to initialize QPainter!";
        return false;
    }

    // Set font for the PDF
    painter.setFont(QFont("Arial", 12)); // Set a larger font for readability

    // Y-position for drawing data
    int yPosition = 100;  // Start from the top of the page
    int xPositionID = 100;  // Start for ID column
    int xPositionDebut = xPositionID + 1000;  // Space between columns (1000 = 300 + 700)
    int xPositionFin = xPositionDebut + 1000;  // Space between columns (1000 = 300 + 700)

    // Draw the header row with the same spacing
    painter.drawText(xPositionID, yPosition, "ID");  // ID Header
    painter.drawText(xPositionDebut, yPosition, "D.J");  // DEBUT JOURNEE Header
    painter.drawText(xPositionFin, yPosition, "F.J");  // FIN JOURNEE Header

    // Increase y-position to leave space between the header and data rows
    yPosition += 300;  // Increased vertical space between rows

    // Loop through the rows of the model and export each row to the PDF
    int rowCount = ui->tableView_pointage->model()->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex indexID = ui->tableView_pointage->model()->index(row, 0);  // ID column
        QModelIndex indexDebut = ui->tableView_pointage->model()->index(row, 1);  // DEBUT JOURNEE column
        QModelIndex indexFin = ui->tableView_pointage->model()->index(row, 2);  // FIN JOURNEE column

        QString id = ui->tableView_pointage->model()->data(indexID).toString();
        QString debutJournee = ui->tableView_pointage->model()->data(indexDebut).toString();
        QString finJournee = ui->tableView_pointage->model()->data(indexFin).toString();

        // Draw each row of data with added horizontal and vertical spacing
        painter.drawText(xPositionID, yPosition, id);  // ID
        painter.drawText(xPositionDebut, yPosition, debutJournee);  // DEBUT JOURNEE
        painter.drawText(xPositionFin, yPosition, finJournee);  // FIN JOURNEE

        // Increase y-position to move to the next line with more space between rows
        yPosition += 300;  // Increased vertical space between rows

        // If the yPosition exceeds the page height, create a new page
        if (yPosition > writer.height() - 50) {
            writer.newPage();
            yPosition = 100; // Reset yPosition for the new page
        }
    }

    // End the PDF drawing
    painter.end();

    return true;  // Indicate success
}
void MainWindow::on_pushButton_employe_clicked() {
    // Make sure the current window (if it's any other window) is hidden
    this->show();  // Show the MainWindow again (if it was hidden)
}
//For example, if you have a different window, make sure to hide it before bringing MainWindow back:
//void SomeOtherWindow::on_pushButton_something_clicked() {
//MainWindow *mainWindow = new MainWindow();  // Or use the existing instance
//mainWindow->show();  // Show the MainWindow
//this->hide();  // Hide the current window (e.g., SomeOtherWindow)
//}
