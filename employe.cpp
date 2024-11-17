#include "employe.h"
#include <QDebug>
#include <QSqlError>

employe::employe(int id, QString nom , QString prenom,  QString poste,int salaire, QDate date_embauche)
{
this -> id=id;
this -> nom=nom;
this -> prenom=prenom;
this -> salaire=salaire;
this -> date_embauche=date_embauche;
this -> poste=poste;
}

bool employe::ajouter()
{
      QSqlQuery query ;
      QString res = QString::number(id);
      //bch nhot kal requete mtaa l securité
      query.prepare("insert into employe(id,nom,prenom,poste,salaire,date_embauche)" "values(:id, :nom, :prenom, :poste, :salaire, :date_embauche)");
      //les variables liéés
      query.bindValue(":id",res);
      query.bindValue(":nom",nom);
      query.bindValue(":prenom",prenom);
      query.bindValue(":poste",poste);
      query.bindValue(":salaire",salaire);
      query.bindValue(":date_embauche",date_embauche);


      return query.exec(); // executer la req

}

QSqlQueryModel* employe::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM employe");

    model->setHeaderData(0,Qt::Horizontal, QObject::tr("id"));

    model->setHeaderData (1,Qt:: Horizontal, QObject::tr("nom"));

    model->setHeaderData (2,Qt:: Horizontal, QObject::tr("prenom"));

    return model;

    }


bool employe::supprimer(int id)
{

    QSqlQuery query;
    QString res=QString::number(id);
    query.prepare("delete from employe where ID= :id");
    query.bindValue(":id",res);
    return query.exec();
}

bool employe::update(int id, QString nom, QString prenom, QString poste, int salaire, QDate date_embauche) {
    QSqlQuery query;
    query.prepare("UPDATE  employe SET nom = :nom, prenom = :prenom, poste = :poste, salaire = :salaire, date_embauche = :date_embauche WHERE id = :id");

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":poste", poste);
    query.bindValue(":salaire", salaire);
    query.bindValue(":date_embauche", date_embauche);

    return query.exec(); // Execute the update query
}


QMap<QString, int> employe::statistiquesParPoste() {
    QMap<QString, int> stats;

    QSqlQuery query;
    query.prepare("SELECT poste, COUNT(*) FROM employe GROUP BY poste");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête SQL pour les statistiques : " << query.lastError().text();
        return stats; // Si la requête échoue, retourne une carte vide
    }

    while (query.next()) {
        QString poste = query.value(0).toString();
        int nombreEmployes = query.value(1).toInt();
        stats[poste] = nombreEmployes;
        qDebug() << "Poste:" << poste << "Nombre d'employés:" << nombreEmployes;
    }

    if (stats.isEmpty()) {
        qDebug() << "Aucune donnée trouvée pour les statistiques.";
    } else {
        qDebug() << "Statistiques récupérées : " << stats;
    }

    return stats;
}





QSqlQueryModel* employe::rechercher(QString searchKey) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM employe WHERE id LIKE :searchKey OR nom LIKE :searchKey OR prenom LIKE :searchKey");
    query.bindValue(":searchKey", "%" + searchKey + "%");

    if (query.exec()) {
        model->setQuery(query);
    } else {
        model->clear();
    }
    return model;
}




QSqlQueryModel* employe::trierParSalaire() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM employe ORDER BY salaire DESC");
    return model;
}

QMap<QString, int> employe::statistiquesParSalaire() {
    QMap<QString, int> stats;
    QSqlQuery query;

    // Requête simplifiée pour tester les résultats
    query.prepare("SELECT salaire FROM employe");

    if (!query.exec()) {
        qDebug() << "Erreur SQL : " << query.lastError().text();
        return stats; // Retourner un QMap vide si la requête échoue
    }

    // Vérification du nombre de lignes retournées
    qDebug() << "Nombre de lignes dans la table employe : " << query.size();

    while (query.next()) {
        double salaire = query.value(0).toDouble();
        qDebug() << "Salaire : " << salaire;

        // Classification des salaires dans des tranches simples pour le débogage
        QString tranche;
        if (salaire < 2000) {
            tranche = "Moins de 2000";
        } else if (salaire >= 2000 && salaire <= 4000) {
            tranche = "2000-4000";
        } else if (salaire >= 4001 && salaire <= 6000) {
            tranche = "4001-6000";
        } else {
            tranche = "Plus de 6000";
        }

        // Comptage des employés dans chaque tranche
        stats[tranche]++;
    }

    return stats;
}
