#include "gestcommande.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QMessageBox>
#include <QFile>

Commande::Commande() : id(0), date(""), statut(""), modePaiement("") {
    // Constructeur par défaut
}

Commande::Commande(int id, QString date, QString statut, QString modePaiement)
    : id(id), date(date), statut(statut), modePaiement(modePaiement) {
    // Constructeur avec paramètres
}

bool Commande::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO COMMANDE (ID, DATE_1, STATUE, MODE_PAIEMENT) "
                  "VALUES (:ID, :DATE_1, :STATUE, :MODE_PAIEMENT)");
    query.bindValue(":ID", id);
    query.bindValue(":DATE_1", date);
    query.bindValue(":STATUE", statut);
    query.bindValue(":MODE_PAIEMENT", modePaiement);

    return query.exec();
}

QSqlQueryModel* Commande::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery("SELECT * FROM COMMANDE");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("DATE_1"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("STATUE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("MODE_PAIEMENT"));

    return model;
}

bool Commande::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM COMMANDE WHERE ID = :ID");
    query.bindValue(":ID", id);

    return query.exec();
}

bool Commande::modifier() {
    QSqlQuery query;
    query.prepare("UPDATE COMMANDE SET DATE_1 = :DATE_1, STATUE = :STATUE, MODE_PAIEMENT = :MODE_PAIEMENT "
                  "WHERE ID = :ID");
    query.bindValue(":ID", id);
    query.bindValue(":DATE_1", date);
    query.bindValue(":STATUE", statut);
    query.bindValue(":MODE_PAIEMENT", modePaiement);

    return query.exec();
}
QSqlQueryModel* Commande::afficherParID(bool ascending)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QString queryStr = ascending
            ? "SELECT * FROM COMMANDE ORDER BY ID ASC"
            : "SELECT * FROM COMMANDE ORDER BY ID DESC";

    model->setQuery(queryStr);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("DATE_1"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("STATUE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("MODE_PAIEMENT"));

    return model;
}
QSqlQueryModel* Commande::rechercherParID(int id)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM COMMANDE WHERE ID = :id");
    query.bindValue(":id", id);
    query.exec();

    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("DATE_1"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("STATUE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("MODE_PAIEMENT"));

    return model;
}
QMap<QString, int> Commande::getStatisticsByStatus()
{
    QMap<QString, int> stats;
    QSqlQuery query;

    // Compter les commandes avec le statut "En cours"
    if (query.exec("SELECT COUNT(*) FROM COMMANDE WHERE STATUE = 'En cours'")) {
        if (query.next()) {
            stats["En cours"] = query.value(0).toInt();
        } else {
            stats["En cours"] = 0;
        }
    } else {
        qDebug() << "Erreur SQL (En cours):" << query.lastError().text();
        stats["En cours"] = 0;
    }

    // Compter les commandes avec le statut "livre"
    if (query.exec("SELECT COUNT(*) FROM COMMANDE WHERE STATUE = 'livre'")) {
        if (query.next()) {
            stats["livre"] = query.value(0).toInt();
        } else {
            stats["livre"] = 0;
        }
    } else {
        qDebug() << "Erreur SQL (livre):" << query.lastError().text();
        stats["livre"] = 0;
    }

    return stats;
}
void Commande::genererRapportPDF(QString cheminFichier)
{
    QPdfWriter pdf(cheminFichier);
    QPainter painter(&pdf);
    int y = 50; // Position verticale de départ
    int lineHeight = 200; // Espacement entre les lignes

    // Titre du rapport
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(100, y, "Rapport des Commandes");
    y += 2 * lineHeight; // Double espacement pour le titre

    // Section 1 : Nombre de commandes par statut
    painter.setFont(QFont("Arial", 12));
    painter.drawText(100, y, "1. Nombre de commandes par statut :");
    y += lineHeight;

    QSqlQuery query;
    query.prepare("SELECT STATUE, COUNT(*) AS NOMBRE FROM COMMANDE WHERE STATUE IN ('Livrée', 'En cours') GROUP BY STATUE");
    if (query.exec()) {
        while (query.next()) {
            QString statut = query.value(0).toString();
            int nombre = query.value(1).toInt();
            painter.drawText(120, y, QString("- %1 : %2").arg(statut).arg(nombre));
            y += lineHeight;
        }
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Erreur SQL"),
                              QObject::tr("La requête a échoué : %1").arg(query.lastError().text()));
        return; // Arrêter en cas d'erreur SQL
    }

    // Section 2 : Modes de paiement les plus utilisés
    y += lineHeight; // Ajout d'un espacement entre les sections
    painter.setFont(QFont("Arial", 12));
    painter.drawText(100, y, "2. Modes de paiement les plus utilisés :");
    y += lineHeight;

    query.prepare("SELECT MODE_PAIEMENT, COUNT(*) AS NOMBRE FROM COMMANDE GROUP BY MODE_PAIEMENT ORDER BY NOMBRE DESC");
    if (query.exec()) {
        while (query.next()) {
            QString mode = query.value(0).toString();
            int nombre = query.value(1).toInt();
            painter.drawText(120, y, QString("- %1 : %2").arg(mode).arg(nombre));
            y += lineHeight;
        }
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Erreur SQL"),
                              QObject::tr("La requête a échoué : %1").arg(query.lastError().text()));
        return; // Arrêter en cas d'erreur SQL
    }

    painter.end();

    // Vérification que le fichier a été créé
    QFile fichier(cheminFichier);
    if (fichier.exists()) {
        QMessageBox::information(nullptr, QObject::tr("Rapport généré"),
                                 QObject::tr("Le rapport a été créé avec succès :\n%1").arg(cheminFichier));
    } else {
        QMessageBox::critical(nullptr, QObject::tr("Erreur"),
                              QObject::tr("Le rapport n'a pas pu être généré."), QMessageBox::Cancel);
    }
}

