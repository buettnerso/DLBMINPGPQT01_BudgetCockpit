#pragma once

#include <QList>
#include <QString>
#include <QDate>

#include "Transaction.h"

// Klasse zur Verwaltung von Transaktionen sowie
// zur Berechnung und Filterung der Finanzdaten

class BudgetManager
{
public:
    // Transaktionen verwalten
    void addTransaction(const Transaction& transaction);
    bool removeTransaction(int index);

    const QList<Transaction>& getAllTransactions() const;


    // Berechnungen für alle gespeicherten Transaktionen
    double calculateIncome() const;
    double calculateExpenses() const;
    double calculateBalance() const;


    // Berechnungen für eine übergebene Liste von Transaktionen
    double calculateIncome(
        const QList<Transaction>& transactionsToCalculate
    ) const;

    double calculateExpenses(
        const QList<Transaction>& transactionsToCalculate
    ) const;

    double calculateBalance(
        const QList<Transaction>& transactionsToCalculate
    ) const;


    // Filterung nach Kategorie
    QList<Transaction> filterByCategory(
        const QString& category
    ) const;

    QList<Transaction> filterByCategory(
        const QList<Transaction>& transactionsToFilter,
        const QString& category
    ) const;


    // Filterung nach Typ (Einnahme / Ausgabe)
    QList<Transaction> filterByType(
        const QString& type
    ) const;

    QList<Transaction> filterByType(
        const QList<Transaction>& transactionsToFilter,
        const QString& type
    ) const;

    // Filterung nach Zeitraum
    QList<Transaction> filterByDateRange(
        const QDate& fromDate,
        const QDate& toDate
    ) const;

    QList<Transaction> filterByDateRange(
        const QList<Transaction>& transactionsToFilter,
        const QDate& fromDate,
        const QDate& toDate
    ) const;

private:
    QList<Transaction> transactions;
};