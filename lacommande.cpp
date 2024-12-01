#include "lacommande.h"
#include "ui_lacommande.h"
#include "gestcommande.h"
#include <QMessageBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QGraphicsScene>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QFileDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "arduino.h"
#include <QSqlError>     // Pour gérer les erreurs SQL
#include <QString>       // Pour manipuler les chaînes de caractères

using namespace QtCharts;
QSerialPort *arduino;

lacommande::lacommande(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::lacommande),
      arduino(new Arduino) // Initialisation d'Arduino
{
    ui->setupUi(this);

    // Tentative de connexion à l'Arduino
    if (arduino->connectToArduino() != 0) {
        QMessageBox::critical(this, "Erreur", "Impossible de se connecter à l'Arduino !");
    } else {
        connect(arduino->getSerial(), &QSerialPort::readyRead, this, &lacommande::onArduinoDataReceived);
        QMessageBox::information(this, "Arduino", "Connexion à l'Arduino réussie !");
    }
    connect(ui->pushButton_CheckEnCours, &QPushButton::clicked, this, &lacommande::sendCommandesParMotToArduino);
}

lacommande::~lacommande()
{
    if (arduino->getSerial()->isOpen()) {
        arduino->disconnectFromArduino();
    }
    delete arduino;
    delete ui;
}

void lacommande::checkCommandesParMot()
{
    // Récupérer le mot depuis la ligne d'édition
    QString searchWord = ui->lineEdit_searchWord->text();

    if (searchWord.isEmpty()) {
        QMessageBox::warning(this, "Recherche vide", "Veuillez entrer un mot à rechercher.");
        return;
    }

    // Préparer la requête SQL pour compter les occurrences
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) AS nombre_commandes FROM COMMANDE WHERE STATUE = :searchWord");
    query.bindValue(":searchWord", searchWord);

    if (query.exec()) {
        if (query.next()) {
            int nombreCommandes = query.value(0).toInt();
            qDebug() << "Nombre de commandes pour '" << searchWord << "': " << nombreCommandes;

            // Afficher le résultat dans une boîte de dialogue
            QMessageBox::information(this, "Statistiques",
                                     QString("Il y a %1 commandes avec le statut '%2'.")
                                     .arg(nombreCommandes)
                                     .arg(searchWord));
        }
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête SQL:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Impossible de récupérer les données.");
    }
}

void lacommande::sendCommandesParMotToArduino()
{
    // Récupérer le mot depuis la ligne d'édition
    QString searchWord = ui->lineEdit_searchWord->text();

    if (searchWord.isEmpty()) {
        QMessageBox::warning(this, "Recherche vide", "Veuillez entrer un mot à rechercher.");
        return;
    }

    // Préparer la requête SQL pour compter les occurrences
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) AS nombre_commandes FROM COMMANDE WHERE STATUE = :searchWord");
    query.bindValue(":searchWord", searchWord);

    if (query.exec()) {
        if (query.next()) {
            int nombreCommandes = query.value(0).toInt();
            QString message = QString("'%1': %2 occurrences").arg(searchWord).arg(nombreCommandes);

            // Envoyer le message à l'Arduino
            if (arduino->sendDataToArduino(message.toUtf8()) == 0) {
                qDebug() << "Message envoyé à l'Arduino :" << message;
                QMessageBox::information(this, "Envoyé",
                                         QString("Message envoyé à l'Arduino :\n%1").arg(message));
            } else {
                qDebug() << "Échec de l'envoi du message à l'Arduino.";
                QMessageBox::critical(this, "Erreur", "Échec de l'envoi à l'Arduino.");
            }
        }
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête SQL :" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Impossible de récupérer les données.");
    }
}

void lacommande::onArduinoDataReceived()
{
    QByteArray data = arduino->receiveDataFromArduino();
    if (!data.isEmpty()) {
        qDebug() << "Données reçues de l'Arduino :" << data;
        ui->textEdit_log->append(QString("Arduino: %1").arg(QString(data)));
    }
}


void lacommande::on_pushButton_ajouter_clicked()
{
    int id = ui->lineEdit_ID->text().toInt();
    QString date = ui->lineEdit_date->text();
    QString statut = ui->lineEdit_statut->text();
    QString modePaiement = ui->lineEdit_ModePaiement->text();

    Commande C(id, date, statut, modePaiement);

    bool test = C.ajouter();

    if (test)
    {
        QMessageBox::information(nullptr, QObject::tr("OK"),
                    QObject::tr("Ajout effectué\n"
                                "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
        ui->tableView->setModel(Etmp.afficher());
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                 QObject::tr("Ajout non effectué.\n"
                             "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
    }
}

void lacommande::on_pushButton_supprimer_clicked()
{
    int id = ui->lineEdit_ID->text().toInt();
    bool test = Etmp.supprimer(id);

    if (test)
    {
        QMessageBox::information(nullptr, QObject::tr("OK"),
                    QObject::tr("Suppression effectuée\n"
                                "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
        ui->tableView->setModel(Etmp.afficher());
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                    QObject::tr("Suppression non effectuée.\n"
                                "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
    }
}

void lacommande::on_pushButton_modifier_clicked()
{
    int id = ui->lineEdit_ID->text().toInt();                   // Identifiant de la commande
    QString date = ui->lineEdit_date->text();                   // Date de la commande
    QString statut = ui->lineEdit_statut->text();               // Statut de la commande
    QString modePaiement = ui->lineEdit_ModePaiement->text();   // Mode de paiement

    Commande C(id, date, statut, modePaiement);

    bool test = C.modifier();

    if (test)
    {
        QMessageBox::information(nullptr, QObject::tr("OK"),
                    QObject::tr("Modification effectuée\n"
                                "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);

        ui->tableView->setModel(C.afficher());
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                    QObject::tr("Modification non effectuée.\n"
                                "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
    }
}
void lacommande::on_pushButton_trier_clicked()
{
    sortAscending = !sortAscending; // Alterner entre ASC et DESC
    ui->tableView->setModel(Etmp.afficherParID(sortAscending));
}
void lacommande::on_pushButton_rechercher_clicked()
{
    int id = ui->lineEdit_rechercheID->text().toInt(); // Récupérer l'ID entré
    ui->tableView->setModel(Etmp.rechercherParID(id)); // Mettre à jour le tableau avec les résultats
}
void lacommande::on_pushButton_Stats_clicked()
{
    // Vérifier si la scène existe, sinon en créer une
    if (ui->graphicsView->scene() == nullptr) {
        ui->graphicsView->setScene(new QGraphicsScene(this));
    } else {
        ui->graphicsView->scene()->clear();  // Nettoyer la scène existante
    }

    // Récupérer les statistiques depuis la base de données
    QMap<QString, int> stats = Etmp.getStatisticsByStatus();

    // Vérifier si des statistiques valides sont récupérées
    if (stats.isEmpty()) {
        QMessageBox::critical(this, "Erreur", "Impossible de récupérer les statistiques.");
        return;
    }

    // Créer les données pour le graphique
    QBarSet *set = new QBarSet("Statue");
    *set << stats["En cours"] << stats["livre"];

    // Créer une série de barres
    QBarSeries *series = new QBarSeries();
    series->append(set);

    // Configurer le graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des commandes par statut");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Ajouter les catégories sur l'axe X
    QStringList categories;
    categories << "En cours" << "livre";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Configurer l'axe Y
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, stats["En cours"] + stats["livre"] + 1);  // Ajuster la portée
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Ajouter le graphique à la scène
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->scene()->addWidget(chartView);

    // Ajuster les marges
    chart->setMargins(QMargins(10, 10, 10, 10));
    chart->legend()->setAlignment(Qt::AlignBottom);
}
void lacommande::on_pushButton_ExportPDF_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer sous",
                            "C:/Users/hibaz/Desktop/HACHOUMA/gestioncommande/commandes_list.pdf",
                            "PDF Files (*.pdf)");

    if (filePath.isEmpty()) {
        return;  // Annuler si aucun chemin n'est choisi
    }

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPagedPaintDevice::A4);
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    painter.setFont(QFont("Arial", 10));

    // Dimensions et marges
    int marginLeft = 100;
    int marginTop = 100;
    int rowHeight = 100;
    int columnWidthID = 180;
    int columnWidthDate = 300;
    int columnWidthStatut = 300;

    int pageHeight = pdfWriter.height() - 400;  // Hauteur totale avec marges

    int yOffset = marginTop;

    // Titre et date
    QString currentDate = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    painter.drawText(marginLeft, yOffset, "Liste des Commandes");
    yOffset += 100;
    painter.drawText(marginLeft, yOffset, "Date d'exportation : " + currentDate);
    yOffset += 180;

    // En-tête du tableau
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(marginLeft, yOffset, "ID");
    painter.drawText(marginLeft + columnWidthID, yOffset, "Date");
    painter.drawText(marginLeft + columnWidthID + columnWidthDate, yOffset, "Statut");
    painter.drawText(marginLeft + columnWidthID + columnWidthDate + columnWidthStatut, yOffset, "Mode de Paiement");
    yOffset += rowHeight;

    painter.setFont(QFont("Arial", 10));  // Revenir à une police normale pour les lignes

    // Requête SQL
    QSqlQuery query("SELECT ID, DATE_1, STATUE, MODE_PAIEMENT FROM COMMANDE");

    while (query.next()) {
        // Vérification si une nouvelle page est nécessaire
        if (yOffset > pageHeight) {
            pdfWriter.newPage();
            yOffset = marginTop;

            // Réécrire l'en-tête sur la nouvelle page
            painter.setFont(QFont("Arial", 21, QFont::Bold));
            painter.drawText(marginLeft, yOffset, "ID");
            painter.drawText(marginLeft + columnWidthID, yOffset, "Date");
            painter.drawText(marginLeft + columnWidthID + columnWidthDate, yOffset, "Statut");
            painter.drawText(marginLeft + columnWidthID + columnWidthDate + columnWidthStatut, yOffset, "Mode de Paiement");
            yOffset += rowHeight;

            painter.setFont(QFont("Arial", 30));
        }

        // Ajouter les données
        painter.drawText(marginLeft, yOffset, query.value(0).toString());  // ID
        painter.drawText(marginLeft + columnWidthID, yOffset, query.value(1).toString());  // Date
        painter.drawText(marginLeft + columnWidthID + columnWidthDate, yOffset, query.value(2).toString());  // Statut
        painter.drawText(marginLeft + columnWidthID + columnWidthDate + columnWidthStatut, yOffset, query.value(3).toString());  // Mode de Paiement
        yOffset += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, "Fichier Enregistré", "PDF enregistré dans : " + filePath);
}
void lacommande::genererRapport()
{
    QString cheminRapport = "C:/Users/hibaz/Desktop/HACHOUMA/gestioncommande/rapport/rapport.pdf";

    Etmp.genererRapportPDF(cheminRapport);

    QMessageBox::information(nullptr, QObject::tr("Rapport généré"),
                             QObject::tr("Le rapport a été créé avec succès :\n%1")
                             .arg(cheminRapport));
}
/*void lacommande::on_pushButton_SearchAndSend_clicked()
{
    // Récupérer le mot depuis l'interface utilisateur
    QString searchWord = ui->lineEdit_searchWord->text();

    if (searchWord.isEmpty()) {
        QMessageBox::warning(this, "Recherche vide", "Veuillez entrer un mot à rechercher.");
        return;
    }

    // Rechercher dans la base de données
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM COMMANDE WHERE STATUE = :searchWord");
    query.bindValue(":searchWord", searchWord);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur SQL", "Échec de la recherche dans la base de données.");
        qDebug() << "Erreur SQL:" << query.lastError().text();
        return;
    }

    int count = 0;
    if (query.next()) {
        count = query.value(0).toInt();
    }

    // Envoyer le message à l'Arduino
    QString message = QString("'%1': %2 occurrences").arg(searchWord).arg(count);
    if (arduino->sendDataToArduino(message.toUtf8()) == 0) {
        QMessageBox::information(this, "Envoyé", QString("Message envoyé à l'Arduino:\n%1").arg(message));
    } else {
        QMessageBox::critical(this, "Erreur Arduino", "Échec de l'envoi des données à l'Arduino.");
    }
}*/

