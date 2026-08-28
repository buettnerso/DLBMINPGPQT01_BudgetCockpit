#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>

#include "ui_BudgetCockpit.h"
#include "BudgetManager.h"
#include "CategoryManager.h"
#include "Transaction.h"

class BudgetCockpit : public QMainWindow
{
    Q_OBJECT

public:
    BudgetCockpit(QWidget* parent = nullptr);
    ~BudgetCockpit();

private slots:
    // Neue Buchung erfassen
    void addTransaction();

    // Filter
    void applyFilter();
    void resetFilter();

private:
    Ui::BudgetCockpitClass ui;

    // Geschäftslogik
    BudgetManager budgetManager;
    CategoryManager categoryManager;

    // Aktuell in der Tabelle dargestellte Buchungen
    QList<Transaction> displayedTransactions;

    // Initialisierung der Oberfläche
    void initializeGui();

    // Aktualisierung der Darstellung
    void refreshTransactionTable(
        const QList<Transaction>& transactions
    );

    void refreshStatistics(
        const QList<Transaction>& transactions
    );

    void refreshCategories();
};