#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>

#include "ui_BudgetCockpit.h"
#include "BudgetManager.h"
#include "CategoryManager.h"
#include "Transaction.h"
#include "CsvRepository.h"

class QChartView;

class BudgetCockpit : public QMainWindow
{
    Q_OBJECT

public:
    BudgetCockpit(QWidget* parent = nullptr);
    ~BudgetCockpit();

private slots:
    // Neue Buchung erfassen
    void addTransaction();

    // Buchung löschen
    void deleteTransaction();

    // Filter
    void applyFilter();
    void resetFilter();

    // CSV-Persistenz-Datei öffnen
    void openCsvFile();
    void saveCsvFile();


private:
    Ui::BudgetCockpitClass ui;

    // Geschäftslogik
    BudgetManager budgetManager;
    CategoryManager categoryManager;

    // --------------------------------------------------------

    // CSV-Persistenz-Persistenz
    CsvRepository csvRepository;

    // --------------------------------------------------------

    // Aktuell geöffnete Budget-Datei
    QString currentCsvFilePath;

    // --------------------------------------------------------

    // Aktuell in der Tabelle dargestellte Buchungen
    QList<Transaction> displayedTransactions;

    // Aktuell dargestelltes Kreisdiagramm
    QChartView* analysisChartView = nullptr;

    // Status der Filterung
    bool filterActive = false;

    // --------------------------------------------------------

    // Initialisierung der Oberfläche
    void initializeGui();

    // --------------------------------------------------------

    // CSV-Persistenz
    bool saveCurrentCsvFile();

    // --------------------------------------------------------

    // Darstellung aktualisieren
    void refreshTransactionTable(
        const QList<Transaction>& transactions)
    {
        // Tabelle zunächst auf die benötigte
        // Anzahl an Zeilen setzen
        ui.tblTransactions->setRowCount(
            transactions.size()
        );

        // Deutsche Darstellung für Geldbeträge
        const QLocale germanLocale(
            QLocale::German,
            QLocale::Germany
        );

        for (int row = 0;
            row < transactions.size();
            ++row)
        {
            const Transaction& transaction =
                transactions.at(row);

            // Datum
            ui.tblTransactions->setItem(
                row,
                0,
                new QTableWidgetItem(
                    transaction.getDate()
                    .toString("dd.MM.yyyy")
                )
            );

            // Art
            ui.tblTransactions->setItem(
                row,
                1,
                new QTableWidgetItem(
                    transaction.getType()
                )
            );

            // Kategorie
            ui.tblTransactions->setItem(
                row,
                2,
                new QTableWidgetItem(
                    transaction.getCategory()
                )
            );

            // Betrag
            QTableWidgetItem* amountItem =
                new QTableWidgetItem(
                    germanLocale.toString(
                        transaction.getAmount(),
                        'f',
                        2
                    ) + " €"
                );

            amountItem->setTextAlignment(
                Qt::AlignRight |
                Qt::AlignVCenter
            );

            ui.tblTransactions->setItem(
                row,
                3,
                amountItem
            );

            // Beschreibung
            ui.tblTransactions->setItem(
                row,
                4,
                new QTableWidgetItem(
                    transaction.getDescription()
                )
            );
        }

        // Nach dem Neuaufbau keine alte Auswahl übernehmen.
        ui.tblTransactions->clearSelection();

        // Löschen erst wieder nach einer neuen Auswahl erlauben.
        ui.btnDeleteTransaction->setEnabled(false);
    }

    // --------------------------------------------------------

    void refreshStatistics(
        const QList<Transaction>& transactions)
    {
        const double income =
            budgetManager.calculateIncome(
                transactions
            );

        const double expenses =
            budgetManager.calculateExpenses(
                transactions
            );

        const double balance =
            budgetManager.calculateBalance(
                transactions
            );


        const QLocale germanLocale(
            QLocale::German,
            QLocale::Germany
        );


        ui.lblIncomeValue->setText(
            germanLocale.toString(
                income,
                'f',
                2
            ) + " €"
        );


        ui.lblExpenseValue->setText(
            germanLocale.toString(
                expenses,
                'f',
                2
            ) + " €"
        );


        ui.lblBalanceValue->setText(
            germanLocale.toString(
                balance,
                'f',
                2
            ) + " €"
        );


        const int transactionCount =
            transactions.size();

        ui.lblCurrentView->setText(
            QString("%1 %2")
            .arg(transactionCount)
            .arg(
                transactionCount == 1
                ? "Buchung"
                : "Buchungen"
            )
        );
    }

    void refreshCategories();


    void syncBookingFilterToAnalysis();

    void syncAnalysisFilterToBooking();

    // Auswertung / Kreisdiagramm aktualisieren
    void refreshAnalysisChart(
        const QList<Transaction>& transactions
    );
};