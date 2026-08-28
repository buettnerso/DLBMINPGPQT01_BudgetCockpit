#include "stdafx.h"  // Vorabkompilierte Headerdatei
#include "BudgetCockpit.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLocale>

BudgetCockpit::BudgetCockpit(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    initializeGui();

    // Signal-Slot-Verbindungen
    connect(
        ui.btnAddTransaction,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::addTransaction
    );

    connect(
        ui.btnApplyFilter,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::applyFilter
    );

    connect(
        ui.btnResetFilter,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::resetFilter
    );

    connect(
        ui.btnLoadCsv,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::openCsvFile
    );
}


BudgetCockpit::~BudgetCockpit()
{}

void BudgetCockpit::initializeGui()
{
    // Aktuelles Datum für neue Buchungen setzen
    ui.dateTransaction->setDate(QDate::currentDate());

    // Kategorien aus dem CategoryManager laden
    refreshCategories();
}

void BudgetCockpit::refreshCategories()
{
    // Kategorieauswahl für neue Buchungen
    ui.cmbCategory->clear();
    ui.cmbCategory->addItems(
        categoryManager.getCategories()
    );

    // Kategorieauswahl für den Filter
    ui.cmbFilterCategory->clear();
    ui.cmbFilterCategory->addItem("Alle Kategorien");
    ui.cmbFilterCategory->addItems(
        categoryManager.getCategories()
    );
}


// für Testzwecke vorübergehend implementiert

void BudgetCockpit::addTransaction()
{
    const QDate date =
        ui.dateTransaction->date();

    const QString type =
        ui.cmbTransactionType->currentText();

    const QString category =
        ui.cmbCategory->currentText();

    const double amount =
        ui.spnAmount->value();

    const QString description =
        ui.txtDescription->text().trimmed();


    Transaction transaction(
        date,
        type,
        category,
        amount,
        description
    );


    budgetManager.addTransaction(transaction);
}


void BudgetCockpit::applyFilter()
{}


void BudgetCockpit::resetFilter()
{}

//--------------------------------------------------------------
// CSV FUnktionen
//--------------------------------------------------------------

void BudgetCockpit::openCsvFile()
{
    const QString filePath =
        QFileDialog::getOpenFileName(
            this,
            "Budget-Datei öffnen",
            QString(),
            "CSV-Dateien (*.csv);;Alle Dateien (*.*)"
        );


    // Abbrechen wurde geklickt
    if (filePath.isEmpty())
    {
        return;
    }


    QList<Transaction> loadedTransactions;
    QString errorMessage;


    const bool success =
        csvRepository.load(
            filePath,
            loadedTransactions,
            &errorMessage
        );


    if (!success)
    {
        QMessageBox::critical(
            this,
            "CSV-Datei konnte nicht geladen werden",
            errorMessage
        );

        return;
    }


    // Geladene Daten werden zum neuen
    // Datenbestand des BudgetManagers.
    budgetManager.setTransactions(
        loadedTransactions
    );


    // Diese CSV ist ab jetzt die aktive Arbeitsdatei.
    currentCsvFilePath =
        filePath;


    // Aktuelle Ansicht aktualisieren.
    displayedTransactions =
        loadedTransactions;


    // Kategorien aus der geladenen CSV übernehmen.
    for (const Transaction& transaction :
        loadedTransactions)
    {
        categoryManager.addCategory(
            transaction.getCategory()
        );
    }


    // Kategorie-ComboBoxen aktualisieren.
    refreshCategories();


    // Tabelle aktualisieren.
    refreshTransactionTable(
        displayedTransactions
    );


    // Kennzahlen aktualisieren.
    refreshStatistics(
        displayedTransactions
    );


    // Aktuell aktive Datei in der Statusleiste anzeigen.
    statusBar()->showMessage(
        "Aktive Budget-Datei: " +
        QFileInfo(filePath).fileName()
    );


    // Erst jetzt Erfolgsmeldung anzeigen.
    QMessageBox::information(
        this,
        "Budget-Datei geladen",
        QString(
            "%1 Buchungen wurden aus\n%2\ngeladen."
        )
        .arg(loadedTransactions.size())
        .arg(QFileInfo(filePath).fileName())
    );
}