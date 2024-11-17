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

using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(etmp.afficher());
    chronoTimer = new QTimer(this);
    timeElapsed = 0;
    isTimerRunning = false;
       chronoTimer = new QTimer(this);
    connect(chronoTimer,&QTimer::timeout, this, &MainWindow::updateTimer);
     ui->lcdNumber_chrono->display("00:00:00");
     connect(ui->pushButton_chrono, &QPushButton::clicked, this, &MainWindow::toggleTimer);
     // Initialisation du modèle d'historique
         historiqueModel = new QStandardItemModel(this);
         historiqueModel->setHorizontalHeaderLabels({"Action", "Détails", "Date"});
         ui->tableView_historique->setModel(historiqueModel);
        connect(ui->pushButton_statistiques_salaire, &QPushButton::clicked, this, &MainWindow::on_pushButton_statistiques_salaire_clicked);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ajouter_clicked()
{
//recuperer les données
int id=ui->lineEdit_ID->text().toInt();
QString nom=ui->lineEdit_nom->text();
QString prenom=ui->lineEdit_prenom->text();
int salaire=ui->lineEdit_salaire->text().toInt();
QString poste=ui->lineEdit_poste->text();
QDate date = QDate::fromString(ui->lineEdit_date->text(), "yyyy-MM-dd");
employe e(id, nom, prenom, poste, salaire, date);

bool test=e.ajouter();
if(test)
{
    ui->tableView->setModel(etmp.afficher());
    QMessageBox::information(nullptr,QObject::tr("ok"),
                             QObject::tr("ajout effectué\n" "click to exit."));

 updateHistorique("Ajout", "Ajout de l'employé " + nom + " " + prenom);
}
else {
    QMessageBox::critical(nullptr,QObject::tr("not ok"),
                          QObject::tr("ajout non effectué.\n""click cancel to exit"),
                          QMessageBox::Cancel);
}
}
void MainWindow::on_pushButton_supprimer_clicked()
{
    int id=ui->lineEdit_ID->text().toInt();
    bool test=etmp.supprimer(id);
    if(test)
    {
        ui->tableView->setModel(etmp.afficher());
        QMessageBox::information(nullptr,QObject::tr("ok"),
                                 QObject::tr("ajout effectué\n" "click to exit."));

 updateHistorique("Suppression", "Suppression de l'employé avec ID: " + QString::number(id));
    }
    else {
        QMessageBox::critical(nullptr,QObject::tr("not ok"),
                              QObject::tr("ajout non effectué.\n""click cancel to exit"),
                              QMessageBox::Cancel);

}
}

void MainWindow::on_pushButton_update_clicked() {
    int id = ui->lineEdit_ID->text().toInt();
    QString nom = ui->lineEdit_nom->text();
    QString prenom = ui->lineEdit_prenom->text();
    int salaire = ui->lineEdit_salaire->text().toInt();
    QString poste = ui->lineEdit_poste->text();
    QDate date = QDate::fromString(ui->lineEdit_date->text(), "yyyy-MM-dd");

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
        for (int row = 0; row < model->rowCount(); ++row) {
            QString line;
            for (int col = 0; col < model->columnCount(); ++col) {
                line += model->data(model->index(row, col)).toString() + " ";
            }
            painter.drawText(100, yPos, line);
            yPos += 20;
        }

        QMessageBox::information(this, "Exporter PDF", "PDF exporté avec succès.");
    }
}

void MainWindow::on_pushButton_chrono_clicked() {
    if (chronoTimer->isActive()) {
        // Si le chronomètre est déjà actif, on l'arrête (fin de pause)
        chronoPauseDuration += chronoStartTime.msecsTo(QTime::currentTime());  // Ajouter le temps de la pause
        chronoTimer->stop();  // Arrêter le chronomètre
        breakEndTime = QTime::currentTime();  // Enregistrer la fin de la pause
        updatePause();  // Mettre à jour l'affichage du temps de pause
    } else {
        // Si le chronomètre est inactif, on démarre la pause
        breakStartTime = QTime::currentTime();  // Enregistrer le début de la pause
        chronoStartTime = breakStartTime;  // Redémarrer le chronomètre
        chronoTimer->start(1000);  // Démarrer le chronomètre de pause avec une mise à jour toutes les secondes
    }
}




void MainWindow::updatePause() {
    // Temps écoulé depuis le début de la pause
    int elapsedTime = breakStartTime.msecsTo(QTime::currentTime()) + pauseDuration;
    QTime elapsedTimeFormatted = QTime::fromMSecsSinceStartOfDay(elapsedTime); // Formater le temps écoulé
    ui->lcdNumber_chrono->display(elapsedTimeFormatted.toString("hh:mm:ss")); // Afficher le temps formaté sur lcdNumber
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

void MainWindow::on_pushButton_commencer_clicked() {
    startTime = QTime::currentTime();
    ui->label_heureTravail->setText("Heure de début : " + startTime.toString("hh:mm:ss"));
}




void MainWindow::on_pushButton_finJournee_clicked() {
    endTime = QTime::currentTime();
    int totalWorkedTime = startTime.msecsTo(endTime) - pauseDuration; // Temps total travaillé - Durée de la pause
    ui->label_heureTravail->setText("Temps travaillé : " + QTime(0, 0).addMSecs(totalWorkedTime).toString("hh:mm:ss"));
}

void MainWindow::updateChrono() {
    int elapsedTime = chronoStartTime.msecsTo(QTime::currentTime()) + chronoPauseDuration;  // Temps écoulé en millisecondes
    QTime elapsedTimeFormatted = QTime::fromMSecsSinceStartOfDay(elapsedTime); // Formater le temps écoulé
    ui->lcdNumber_chrono->display(elapsedTimeFormatted.toString("hh:mm:ss")); // Afficher le temps formaté sur lcdNumber
}

void MainWindow::toggleTimer()
{
    if (isTimerRunning) {
        // Si le timer est déjà en cours, on l'arrête
        chronoTimer->stop();
    } else {
        // Sinon, on démarre le timer
        chronoTimer->start(1000); // Le timer se déclenche toutes les secondes
    }

    // Alterner l'état du chronomètre
    isTimerRunning = !isTimerRunning;
}

void MainWindow::updateTimer()
{
    // Incrémenter le temps écoulé
    timeElapsed++;

    // Calculer les minutes et secondes
    int minutes = timeElapsed / 60;
    int seconds = timeElapsed % 60;

    // Afficher le temps sur le LCD
    QString timeString = QString::asprintf("%02d:%02d", minutes, seconds);
    ui->lcdNumber_chrono->display(timeString);
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
