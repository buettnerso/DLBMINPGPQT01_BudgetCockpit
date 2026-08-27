#pragma once

#include <QDate>
#include <QList>
#include <QString>

#include <optional>

#include "Transaction.h"


class BudgetManager
{
public:

    // ========================================================
    // Transaktionen
    // ========================================================

    void addTransaction(const Transaction& transaction);
    bool removeTransaction(int index);

    const QList<Transaction>& getAllTransactions() const;


    // ========================================================
    // Berechnungen
    // ========================================================

    double calculateIncome() const;
    double calculateExpenses() const;
    double calculateBalance() const;

    double calculateIncome(
        const QList<Transaction>& transactionsToCalculate
    ) const;

    double calculateExpenses(
        const QList<Transaction>& transactionsToCalculate
    ) const;

    double calculateBalance(
        const QList<Transaction>& transactionsToCalculate
    ) const;


    // ========================================================
    // Einzelne Filter
    // ========================================================

    QList<Transaction> filterByCategory(
        const QString& category
    ) const;

    QList<Transaction> filterByType(
        const QString& type
    ) const;

    QList<Transaction> filterByDateRange(
        const QDate& fromDate,
        const QDate& toDate
    ) const;

    QList<Transaction> filterByAmount(
        std::optional<double> minAmount,
        std::optional<double> maxAmount
    ) const;


    // ========================================================
    // Kombinierte Filterung
    // ========================================================

    QList<Transaction> filterTransactions(
        const QString& category,
        const QString& type,
        const QDate& fromDate,
        const QDate& toDate,
        std::optional<double> minAmount,
        std::optional<double> maxAmount
    ) const;


private:

    // ========================================================
    // Interne Filterfunktionen
    // ========================================================

    QList<Transaction> filterByCategory(
        const QList<Transaction>& transactionsToFilter,
        const QString& category
    ) const;

    QList<Transaction> filterByType(
        const QList<Transaction>& transactionsToFilter,
        const QString& type
    ) const;

    QList<Transaction> filterByDateRange(
        const QList<Transaction>& transactionsToFilter,
        const QDate& fromDate,
        const QDate& toDate
    ) const;

    QList<Transaction> filterByAmount(
        const QList<Transaction>& transactionsToFilter,
        std::optional<double> minAmount,
        std::optional<double> maxAmount
    ) const;


    // ========================================================
    // Daten
    // ========================================================

    QList<Transaction> transactions;
};