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

    // Buchung löschen
    void deleteTransaction();

    // Filter
    void applyFilter();
    void resetFilter();

    // CSV-Datei öffnen
    void openCsvFile();
	void saveCsvFile();


private:
    Ui::BudgetCockpitClass ui;

    // GeschäftslogikB
    BudgetManager budgetManager;
    CategoryManager categoryManager;

    //--------------------------------------------------------------

    // CSV-Persistenz
    CsvRepository csvRepository;

    //--------------------------------------------------------------

    // Aktuell geöffnete Budget-Datei
    QString currentCsvFilePath;

    //--------------------------------------------------------------

    // Aktuell in der Tabelle dargestellte Buchungen
    QList<Transaction> displayedTransactions;

    // Status der Filterung
    bool filterActive = false;

    //--------------------------------------------------------------

    // Initialisierung der Oberfläche
    void initializeGui();

	//--------------------------------------------------------------

    // CSV
    bool saveCurrentCsvFile();

	//--------------------------------------------------------------

    // Aktualisierung der Darstellung
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

        // Beschreibung bekommt den restlichen Platz
        ui.tblTransactions
            ->horizontalHeader()
            ->setStretchLastSection(true);
    }

	//---------------------------------------------------------

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


        ui.lblCurrentView->setText(
            QString::number(
                transactions.size()
            ) + " Buchungen"
        );
    }

    void refreshCategories();
};