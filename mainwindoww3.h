#ifndef MAINWINDOWW3_H
#define MAINWINDOWW3_H

#include <QMainWindow>
#include "services.h"
#include <QtCharts>
#include "ui_mainwindoww3.h"

namespace Ui {
class MainWindoww3;
}

class MainWindoww3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindoww3(QWidget *parent = nullptr);
    ~MainWindoww3();

private slots:
    void on_ajouterButton_clicked();
    void on_supprimerButton_clicked();
    void on_modifierButton_clicked();
    void on_afficherButton_clicked();
    void on_trierButton_clicked();
    void on_rechercheButton_clicked();
    void on_statistiquesButton_clicked();
    void on_exporterButton_clicked();
    void exportDataToPDF();
    void on_remiseButton_clicked();
    void on_convertirButton_clicked();
    void on_serviceButton_clicked();
    void scheduleService();
    void on_pb_commande_clicked() ;



private:
    Ui::MainWindoww3 *ui;
    Services service;
    QPushButton *statistiquesButton;
    MainWindoww3 *serviceWindow;
    void showConfirmationMessage(const QString &message);  // Affiche le message de confirmation


};

#endif // MAINWINDOWW3_H

