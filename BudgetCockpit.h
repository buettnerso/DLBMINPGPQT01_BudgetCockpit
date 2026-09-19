#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>

#include "ui_BudgetCockpit.h"
#include "BudgetManager.h"
#include "CategoryManager.h"
#include "Transaction.h"
#include "CsvRepository.h"

class QChartView;
class QLabel;

class BudgetCockpit : public QMainWindow
{
    Q_OBJECT

public:
    BudgetCockpit(QWidget* parent = nullptr);
    ~BudgetCockpit();

private slots:
    // Buchungen
    void addTransaction();
    void deleteTransaction();

    // Filter
    void applyFilter();
    void resetFilter();

    // Budget-Dateien / CSV-Persistenz
    void createNewCsvFile();
    void openCsvFile();
    void saveCsvFile();

private:
    Ui::BudgetCockpitClass ui;

    // Geschäftslogik
    BudgetManager budgetManager;
    CategoryManager categoryManager;

    // CSV-Persistenz
    CsvRepository csvRepository;
    QString currentCsvFilePath;

    // Permanente Anzeige der aktiven Datei in der Statusleiste
    QLabel* lblActiveCsvFile = nullptr;

    // Aktuell dargestellte Buchungen
    QList<Transaction> displayedTransactions;

    // Aktuelles Kreisdiagramm
    QChartView* analysisChartView = nullptr;

    // Status der Filterung
    bool filterActive = false;

    // Initialisierung / Status
    void initializeGui();
    void updateActiveFileDisplay();

    // Persistenz-Hilfsfunktion
    bool saveCurrentCsvFile();

    // Darstellung
    void refreshTransactionTable(
        const QList<Transaction>& transactions
    );

    void refreshStatistics(
        const QList<Transaction>& transactions
    );

    void refreshCategories();

    // Filter zwischen beiden Reitern synchronisieren
    void syncBookingFilterToAnalysis();
    void syncAnalysisFilterToBooking();

    // Auswertung / Kreisdiagramm
    void refreshAnalysisChart(
        const QList<Transaction>& transactions
    );
};
