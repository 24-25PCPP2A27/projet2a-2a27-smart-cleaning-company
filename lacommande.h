#ifndef LACOMMANDE_H
#define LACOMMANDE_H

#include "commande.h"
#include "connection.h"
#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include<QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "arduinoo.h"




QT_BEGIN_NAMESPACE
namespace Ui { class lacommande; }
QT_END_NAMESPACE

class lacommande : public QMainWindow
{
    Q_OBJECT

public:
    explicit lacommande(QWidget *parent = nullptr);
    ~lacommande();







private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_supprimer_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_trier_clicked();
    void on_pushButton_rechercher_clicked();

    void on_pushButton_Stats_clicked();
   void on_pushButton_ExportPDF_clicked();

    void genererRapport();
    //void envoyerDonneesLCD();
    //void on_pushButton_SearchAndSend_clicked();
    void onArduinoDataReceived();
    void checkCommandesParMot();
    void sendCommandesParMotToArduino();
    void on_pushButton_employe_clicked();
    void on_pushButton_navigate_clicked();
    void on_serviceButton_clicked();







    // Méthode pour ajouter et configurer les widgets








private:
    Ui::lacommande *ui;
    Commande Etmp;
    bool sortAscending = true;
    Arduinoo *arduinoo;



};

#endif // LACOMMANDE_H

