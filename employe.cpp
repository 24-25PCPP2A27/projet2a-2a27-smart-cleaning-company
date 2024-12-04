#include "employe.h"
#include <QDebug>
#include <QSqlError>
#include "arduino.h"
#include<QPainter>
#include <QPdfWriter>
#include <QFileDialog>
#include <QAbstractItemModel>
#include <QFont>

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
      query.bindValue(":date_embauche",date_embauche.toString("dd-MM-yyyy"));


      // Si l'ajout dans la base de données réussit
      if (query.exec()) {
          if (arduino) {
              arduino->increment_add_counter(); // Incrémente le compteur
              if (arduino->get_add_counter() >= 2) {
                  arduino->check_and_activate_buzzer();
                  arduino->write_to_arduino("Buzzer\n"); // Envoie le signal à l'Arduino
                  qDebug() << "Limite atteinte : Buzzer activé.";
                  arduino->reset_add_counter(); // Réinitialise le compteur après le buzz
              }
          }
          return true;
      }

      return false;

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
    query.bindValue(":date_embauche", date_embauche.toString("dd-MM-yyyy"));

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
void employe::verifierEtActiverBuzzer()
{
    static int compteurAjouts = 0;  // Compteur statique pour suivre les ajouts
    compteurAjouts++;

    qDebug() << "Compteur d'ajouts : " << compteurAjouts;

    // Si le compteur atteint ou dépasse 5
    if (compteurAjouts >= 2) {
        qDebug() << "Limite atteinte : Buzzer activé.";

        // Si l'Arduino est connecté, envoyer la commande "1" pour activer le buzzer
        if (arduino) {
            QByteArray dataToSend = "1";  // Commande pour activer le buzzer
            arduino->write_to_arduino(dataToSend);
        } else {
            qDebug() << "Arduino non initialisé.";
        }

        // Réinitialiser le compteur après avoir atteint la limite
        compteurAjouts = 0;
    }
}


bool employe::idExiste(int id) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM employe WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0; // Retourne vrai si l'ID existe
    }
    return false; // Sinon, faux
}

QVector<QVector<QString>> employe::getPointageTable() const {
    return pointageTable;
}



// Ajouter un pointage (début ou fin de journée)
bool employe::ajouterPointage(int id, const QString& debutJournee, const QString& finJournee) {
    for (QVector<QString>& ligne : pointageTable) {
        if (ligne[0] == QString::number(id)) {
            // Si l'employé a déjà pointé, on met à jour l'heure de fin
            if (!finJournee.isEmpty() && ligne[2].isEmpty()) {
                ligne[2] = finJournee; // Mise à jour de l'heure de fin
                return true;
            }
        }
    }

    // Si l'employé n'a pas pointé, on ajoute une nouvelle ligne avec l'heure de début
    if (!debutJournee.isEmpty()) {
        pointageTable.append({QString::number(id), debutJournee, ""}); // Ajout avec l'heure de début
        return true;
    }

    return false; // Échec si aucune condition n'est remplie
}

// Récupérer tous les pointages
QVector<QVector<QString>> employe::getPointage() const {
    return pointageTable;
}

// Obtenir une référence au tableau des pointages
QVector<QVector<QString>>& employe::getPointageTableRef() {
    return pointageTable;
}

