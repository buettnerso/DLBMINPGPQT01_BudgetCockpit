#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BudgetCockpit.h"

class BudgetCockpit : public QMainWindow
{
    Q_OBJECT

public:
    BudgetCockpit(QWidget *parent = nullptr);
    ~BudgetCockpit();

private:
    Ui::BudgetCockpitClass ui;
};

