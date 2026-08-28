#include "stdafx.h"  // Vorabkompilierte Headerdatei
#include "BudgetCockpit.h"

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
{}


void BudgetCockpit::applyFilter()
{}


void BudgetCockpit::resetFilter()
{}