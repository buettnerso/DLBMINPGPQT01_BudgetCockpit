#include "stdafx.h"
#include "BudgetManager.h"

// Implementierung der Methoden der BudgetManager-Klasse

void BudgetManager::addTransaction(const Transaction& transaction)
{
    transactions.append(transaction);
}

bool BudgetManager::removeTransaction(int index)
{
    if (index < 0 || index >= transactions.size())
    {
        return false;
    }

    transactions.removeAt(index);
    return true;
}

const QList<Transaction>& BudgetManager::getAllTransactions() const
{
    return transactions;
}

double BudgetManager::calculateIncome() const
{
    return calculateIncome(transactions);
}

double BudgetManager::calculateExpenses() const
{
    return calculateExpenses(transactions);
}

double BudgetManager::calculateBalance() const
{
    return calculateBalance(transactions);
}

double BudgetManager::calculateIncome(
    const QList<Transaction>& transactionsToCalculate) const
{
    double income = 0.0;

    for (const Transaction& transaction : transactionsToCalculate)
    {
        if (transaction.getType() == "Einnahme")
        {
            income += transaction.getAmount();
        }
    }

    return income;
}

double BudgetManager::calculateExpenses(
    const QList<Transaction>& transactionsToCalculate) const
{
    double expenses = 0.0;

    for (const Transaction& transaction : transactionsToCalculate)
    {
        if (transaction.getType() == "Ausgabe")
        {
            expenses += transaction.getAmount();
        }
    }

    return expenses;
}

double BudgetManager::calculateBalance(
    const QList<Transaction>& transactionsToCalculate) const
{
    return calculateIncome(transactionsToCalculate)
        - calculateExpenses(transactionsToCalculate);
}

QList<Transaction> BudgetManager::filterByCategory(   // Filterung nach Kategorie für alle gespeicherten Transaktionen
    const QString& category) const
{
    return filterByCategory(transactions, category);
}



QList<Transaction> BudgetManager::filterByCategory(
    const QList<Transaction>& transactionsToFilter,
    const QString& category) const
{
    QList<Transaction> filteredTransactions;

    QString cleanedCategory = category.trimmed();

    // Keine Kategorie angegeben:
    // Es wird kein Kategorie-Filter angewendet.
    if (cleanedCategory.isEmpty())
    {
        return transactionsToFilter;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        if (transaction.getCategory().compare(
            cleanedCategory,
            Qt::CaseInsensitive) == 0)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}

QList<Transaction> BudgetManager::filterByType(
    const QString& type) const
{
    return filterByType(transactions, type);
}

QList<Transaction> BudgetManager::filterByType(
    const QList<Transaction>& transactionsToFilter,
    const QString& type) const
{
    QList<Transaction> filteredTransactions;

    QString cleanedType = type.trimmed();

    // Kein Typ angegeben:
    // Es wird kein Typ-Filter angewendet.
    if (cleanedType.isEmpty())
    {
        return transactionsToFilter;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        if (transaction.getType().compare(
            cleanedType,
            Qt::CaseInsensitive) == 0)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}

QList<Transaction> BudgetManager::filterByDateRange(
    const QDate& fromDate,
    const QDate& toDate) const
{
    return filterByDateRange(
        transactions,
        fromDate,
        toDate
    );
}

QList<Transaction> BudgetManager::filterByDateRange(
    const QList<Transaction>& transactionsToFilter,
    const QDate& fromDate,
    const QDate& toDate) const
{
    QList<Transaction> filteredTransactions;

    // Kein Datumsfilter gesetzt:
    // Alle Transaktionen zurückgeben.
    if (!fromDate.isValid() && !toDate.isValid())
    {
        return transactionsToFilter;
    }

    // Ungültiger Zeitraum:
    // Startdatum liegt nach dem Enddatum.
    if (fromDate.isValid() &&
        toDate.isValid() &&
        fromDate > toDate)
    {
        return filteredTransactions;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        const QDate transactionDate = transaction.getDate();

        bool matchesFromDate =
            !fromDate.isValid() ||
            transactionDate >= fromDate;

        bool matchesToDate =
            !toDate.isValid() ||
            transactionDate <= toDate;

        if (matchesFromDate && matchesToDate)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}

QList<Transaction> BudgetManager::filterByAmount(
    std::optional<double> minAmount,
    std::optional<double> maxAmount) const
{
    return filterByAmount(
        transactions,
        minAmount,
        maxAmount
    );
}

QList<Transaction> BudgetManager::filterByAmount(
    const QList<Transaction>& transactionsToFilter,
    std::optional<double> minAmount,
    std::optional<double> maxAmount) const
{
    QList<Transaction> filteredTransactions;

    // Keine Betragsgrenzen gesetzt:
    // Kein Betragsfilter wird angewendet.
    if (!minAmount.has_value() &&
        !maxAmount.has_value())
    {
        return transactionsToFilter;
    }

    // Ungültiger Wertebereich:
    // Mindestbetrag ist größer als Höchstbetrag.
    if (minAmount.has_value() &&
        maxAmount.has_value() &&
        minAmount.value() > maxAmount.value())
    {
        return filteredTransactions;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        const double amount = transaction.getAmount();

        bool matchesMinimum =
            !minAmount.has_value() ||
            amount >= minAmount.value();

        bool matchesMaximum =
            !maxAmount.has_value() ||
            amount <= maxAmount.value();

        if (matchesMinimum && matchesMaximum)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}

QList<Transaction> BudgetManager::filterTransactions(
    const QString& category,
    const QString& type,
    const QDate& fromDate,
    const QDate& toDate,
    std::optional<double> minAmount,
    std::optional<double> maxAmount) const
{
    QList<Transaction> filteredTransactions = transactions;

    filteredTransactions =
        filterByCategory(
            filteredTransactions,
            category
        );

    filteredTransactions =
        filterByType(
            filteredTransactions,
            type
        );

    filteredTransactions =
        filterByDateRange(
            filteredTransactions,
            fromDate,
            toDate
        );

    filteredTransactions =
        filterByAmount(
            filteredTransactions,
            minAmount,
            maxAmount
        );

    return filteredTransactions;
}