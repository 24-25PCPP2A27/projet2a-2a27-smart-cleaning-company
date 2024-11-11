#include "employe.h"

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
    query.prepare("UPDATE employe SET nom = :nom, prenom = :prenom, poste = :poste, salaire = :salaire, date_embauche = :date_embauche WHERE ID = :ID");

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":poste", poste);
    query.bindValue(":salaire", salaire);
    query.bindValue(":date_embauche", date_embauche);

    return query.exec(); // Execute the update query
}





