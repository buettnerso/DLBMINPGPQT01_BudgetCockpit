#include "stdafx.h"
#include "BudgetCockpit.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    BudgetCockpit window;
    window.show();

    return app.exec();
}

