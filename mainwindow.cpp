#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "employe.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(etmp.afficher());
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


