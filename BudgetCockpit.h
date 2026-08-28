#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>

#include "ui_BudgetCockpit.h"
#include "BudgetManager.h"
#include "CategoryManager.h"
#include "Transaction.h"
#include "CsvRepository.h"

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

    // GeschäftslogikB
    BudgetManager budgetManager;
    CategoryManager categoryManager;

    // CSV-Persistenz
    CsvRepository csvRepository;

    // Aktuell geöffnete Budget-Datei
    QString currentCsvFilePath;

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