#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "employe.h"
#include <QTimer>  // Pour gérer le chronomètre
#include <QLCDNumber>  // Pour afficher le temps
#include <QList>
#include <QDateTime>
#include <QStandardItemModel>

struct HistoriqueAction {
    QString action;  // Type d'action (ajout, suppression)
    QString details; // Détails de l'action
    QString date;    // Date et heure de l'action
};



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_supprimer_clicked();
    void on_pushButton_update_clicked();

    // Nouvelles fonctionnalités
       void on_pushButton_rechercher_clicked();
       void on_pushButton_trier_clicked();
       void on_pushButton_statistiques_clicked();
       void on_pushButton_exporterPDF_clicked();

    // Chronomètre
       void on_pushButton_chrono_clicked();  // Bouton pour démarrer/arrêter
       void updateChrono();  // Mise à jour du temps
    void updateHistorique(const QString &action, const QString &details);
    void on_pushButton_commencer_clicked();  // Fonction pour commencer le travail
       void on_pushButton_finJournee_clicked(); // Fonction pour marquer la fin de la journée
       void updatePause();
       void toggleTimer();
         void updateTimer();
         void on_pushButton_statistiques_salaire_clicked();
private:
    Ui::MainWindow *ui;
    employe etmp;
    // Chronomètre
    QTimer *chronoTimer;
      int timeElapsed; // Temps écoulé en secondes
     bool isTimerRunning;
    QList<HistoriqueAction> historiqueList;  // Liste pour l'historique
        QStandardItemModel *historiqueModel;     // Modèle pour l'historique
        QTime startTime;  // Heure de début de travail
        QTime breakStartTime;  // Heure de début de la pause
        QTime breakEndTime;  // Heure de fin de la pause
        QTime endTime;  // Heure de fin de journée
        int pauseDuration = 0;  // Durée de la pause en millisecondes
        QTimer *pauseTimer;  // Timer pour la pause
              // Le timer pour le chronomètre
    QTime chronoStartTime;              // Temps de démarrage du chronomètre
    int chronoPauseDuration;           // Durée accumulée des pauses

};
#endif // MAINWINDOW_H
