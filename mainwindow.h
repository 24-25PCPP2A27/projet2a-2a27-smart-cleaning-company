#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "services.h"
#include <QtCharts>
#include "arduino.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void handleArduinoSignal(int boutonId);  // Slot pour gérer le signal de l'Arduino


private:
    Ui::MainWindow *ui;
    Services service;
    QPushButton *statistiquesButton;
    void updateProductQuantity(int produitId);  // Cette fonction est déjà définie
    Arduino *arduino;  // Objet Arduino pour la gestion de la communication avec l'Arduino
};

#endif // MAINWINDOW_H


