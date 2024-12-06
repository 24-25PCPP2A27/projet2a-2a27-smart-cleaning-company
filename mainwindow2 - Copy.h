#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include "produit.h"
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget *parent = nullptr);
    ~MainWindow2();
    void afficher();

private slots:
  void on_pushButton_back_clicked();
  void on_pushButton_employe_clicked();
  void on_pushButton_showStats_clicked();
  void on_pushButton_ajouter_clicked();
  void on_pushButton_2_clicked();
  void on_updateButton_clicked();
  void on_searchButton_clicked();
  void on_exporterButton_clicked();
  void on_sortByIdButton_clicked();
  void on_exportToCSVButton_clicked();
  void on_checkStockButton_clicked();
  void initSerialPort();


private:
    Ui::MainWindow2 *ui;
     QSerialPort* serial;


};

#endif // MAINWINDOW2_H
