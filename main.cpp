#include "lacommande.h"

#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Connection c;

    bool test = c.createconnection();
    lacommande w;

    if (test)
    {
        w.show();
        QMessageBox::information(nullptr, QObject::tr("Base de données ouverte"),
            QObject::tr("Connexion réussie.\n"
                        "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Base de données non ouverte"),
            QObject::tr("Échec de la connexion.\n"
                        "Cliquez sur Annuler pour quitter."), QMessageBox::Cancel);
    }

    return a.exec();
}
