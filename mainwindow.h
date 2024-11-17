#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include "produit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_ajouter_clicked(); // Slot for adding a product
    void on_pushButton_2_clicked();       // Slot for deleting a product
    void on_updateButton_clicked();       // Slot for updating a product
    void on_searchButton_clicked();       // Slot for searching products
    void on_exporterButton_clicked();     // Slot for exporting products to PDF
    void on_sortByIdButton_clicked(); // Slot for sorting products by ID (new)
    void on_exportToCSVButton_clicked();
    void on_pushButton_showStats_clicked();


private:
    Ui::MainWindow *ui;
    Produit currentProduit;               // Current product being manipulated
    QSqlQueryModel *model;                // Model for displaying products

    void afficher();                      // Load products into the model
};

#endif // MAINWINDOW_H



