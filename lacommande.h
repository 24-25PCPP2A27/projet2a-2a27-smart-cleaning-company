#ifndef LACOMMANDE_H
#define LACOMMANDE_H

#include "gestcommande.h"
#include "connection.h"
#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include<QPushButton>



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


    // Méthode pour ajouter et configurer les widgets








private:
    Ui::lacommande *ui;
    Commande Etmp;
    bool sortAscending = true;
    QComboBox *languageSelector;  // Sélecteur de langue

        QPushButton *translateButton;   // Bouton pour traduire
        QVBoxLayout *layoutPrincipal;
};

#endif // LACOMMANDE_H
