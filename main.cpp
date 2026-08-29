#include "stdafx.h"
#include "BudgetCockpit.h"
#include <QtWidgets/QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Name der Anwendung
    app.setApplicationName("Budget-Cockpit");

    // Anwendungssymbol aus den Qt-Ressourcen laden
    app.setWindowIcon(QIcon(":/BudgetCockpit/BudgetCockpit.png"));

    BudgetCockpit window;
    window.show();

    return app.exec();
}

