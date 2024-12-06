#include "services.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QPrinter>
#include <QTextDocument>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QPdfWriter>   // Pour utiliser QPdfWriter
#include <QPainter>     // Pour utiliser QPainter
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QtCharts>

Services::Services() : id(0), nom(""), categorie(""), prix(0.0), description("") {}
Services::Services(int id, QString nom, QString categorie, double prix, QString description)
    : id(id), nom(nom), categorie(categorie), prix(prix), description(description) {}

// Ajouter un service
bool Services::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO services (id, nom, categorie, prix, description) "
                  "VALUES (:id, :nom, :categorie, :prix, :description)");

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);
    query.bindValue(":prix", prix);
    query.bindValue(":description", description);

    if (!query.exec()) {
        qDebug() << "Erreur SQL lors de l'ajout : " << query.lastError().text();
        return false;
    }
    return true;
}
QSqlQueryModel* Services::trierParPrix()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM services ORDER BY prix ASC");
    return model;
}

// Rechercher un service par nom
QSqlQueryModel* Services::rechercherParNom(const QString& nom)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM services WHERE nom LIKE :nom");
    query.bindValue(":nom", "%" + nom + "%");

    if (!query.exec()) {
        qDebug() << "Erreur SQL lors de la recherche : " << query.lastError().text();
        return nullptr;
    }

    model->setQuery(query);
    return model;
}

// Supprimer un service
bool Services::supprimer(int id)
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM services WHERE id = :id");
    checkQuery.bindValue(":id", id);
    if (!checkQuery.exec()) {
        qDebug() << "Erreur SQL lors de la vérification de l'ID : " << checkQuery.lastError().text();
        return false;
    }
    checkQuery.next();
    if (checkQuery.value(0).toInt() == 0) {
        return false;  // Service non trouvé
    }

    QSqlQuery query;
    query.prepare("DELETE FROM services WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

// Mettre à jour un service
bool Services::mettreAJour()
{
    QSqlQuery query;
    query.prepare("UPDATE services SET nom = :nom, categorie = :categorie, prix = :prix, description = :description WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":categorie", categorie);
    query.bindValue(":prix", prix);
    query.bindValue(":description", description);
    return query.exec();
}

// Afficher tous les services
QSqlQueryModel* Services::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM services");
    return model;
}

// Exporter les services vers un fichier PDF
bool Services::exportToPDF(const QString &filePath) {
    QTextDocument document;
    QString htmlContent;

    htmlContent.append("<h1>Rapport des Services</h1>");
    htmlContent.append("<table border='1' cellspacing='0' cellpadding='5'>");
    htmlContent.append("<tr><th>ID</th><th>Nom</th><th>Catégorie</th><th>Prix</th><th>Description</th></tr>");

    QSqlQuery query("SELECT * FROM services");
    if (!query.exec()) {
        qDebug() << "Erreur SQL lors de la récupération des services : " << query.lastError().text();
        return false;  // Retourner false si la requête échoue
    }

    while (query.next()) {
        htmlContent.append("<tr>");
        htmlContent.append("<td>" + query.value("id").toString() + "</td>");
        htmlContent.append("<td>" + query.value("nom").toString() + "</td>");
        htmlContent.append("<td>" + query.value("categorie").toString() + "</td>");
        htmlContent.append("<td>" + query.value("prix").toString() + "</td>");
        htmlContent.append("<td>" + query.value("description").toString() + "</td>");
        htmlContent.append("</tr>");
    }
    htmlContent.append("</table>");

    document.setHtml(htmlContent);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);

    qDebug() << "Tentative de génération du PDF vers : " << filePath;

    // Tentative d'impression
    document.print(&printer);

    // Vérifier si le fichier a été généré correctement
    QFileInfo checkFile(filePath);
    if (!checkFile.exists() || checkFile.size() <= 0) {
        qDebug() << "Erreur de génération du fichier PDF.";
        return false;  // Retourner false si le fichier PDF n'a pas été généré correctement
    }

    qDebug() << "PDF généré avec succès à : " << filePath;
    return true;
}



QMap<QString, int> Services::getStatistiquesParCategorie() {
    QMap<QString, int> stats;

    QSqlQuery query;
    query.exec("SELECT categorie FROM services");

    while (query.next()) {
        QString categorie = query.value(0).toString();
        stats[categorie] = stats.value(categorie, 0) + 1;
    }

    return stats;
}

// Génère un graphique des statistiques par catégorie
// Génère un graphique des statistiques par catégorie
// Génère un graphique des statistiques par catégorie
QtCharts::QChartView* Services::genererGraphiqueStatistiques() {
    QMap<QString, int> stats = getStatistiquesParCategorie();

    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    QtCharts::QBarSet *set = new QtCharts::QBarSet("Services");

    QStringList categories;
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        categories << it.key();
        *set << it.value();
    }

    series->append(set);

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Services par Catégorie");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    // Création de l'axe X avec les catégories
    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->setAxisX(axisX, series);

    // Création de l'axe Y avec des valeurs entières
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    int maxValue = *std::max_element(stats.begin(), stats.end());  // Valeur maximale des services

    // Définir la plage de l'axe Y
    axisY->setRange(0, maxValue);

    // Définir le nombre de ticks (si on veut des ticks entiers)
    axisY->setTickCount(maxValue + 1);  // +1 pour inclure 0

    chart->setAxisY(axisY, series);

    // Créer et afficher le graphique
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}
double Services::appliquerRemise(double pourcentage) {
    if (pourcentage < 0 || pourcentage > 100) {
        return prix; // Remise invalide, retourne le prix original
    }
    double remise = (prix * pourcentage) / 100;
    prix -= remise; // Réduit le prix
    return prix;    // Retourne le nouveau prix
}
bool Services::updateProductQuantity(int produitId, int nouvelleQuantite)
{
    QSqlQuery query;
    query.prepare("UPDATE produits SET \"QUANTITE_EN_STOCK\" = :quantite WHERE id = :id");
    query.bindValue(":quantite", nouvelleQuantite);
    query.bindValue(":id", produitId);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la mise à jour de la quantité du produit: " << query.lastError().text();
        return false;
    } else {
        qDebug() << "Quantité du produit avec ID " << produitId << " mise à jour à " << nouvelleQuantite;
        return true;
    }
}
