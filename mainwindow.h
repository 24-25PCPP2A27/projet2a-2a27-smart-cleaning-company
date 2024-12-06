#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include "produit.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTimer>
#include <QLineEdit>
#include <QInputDialog>
#include <QPushButton>


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
    void on_checkStockButton_clicked();
    void onCheckButtonClicked();
    void initSerialPort();
    void checkStockLevels();
    void onProductUpdated();
    void onModifyButtonClicked();
    void findAndModifyProduct(int productId);
    void updateProductInDatabase(int productId, const QString &newName, const QString &newCategory, int newQuantity, double newPrice);



private:
    Ui::MainWindow *ui;
    Produit currentProduit;               // Current product being manipulated
    QSqlQueryModel *model;                // Model for displaying products
    QSerialPort *serial;

    void afficher();
    void showRestockAlert(const QString& productName);

    QPushButton *modifyButton;  // Modify button to trigger stock update
};

#endif // MAINWINDOW_H



