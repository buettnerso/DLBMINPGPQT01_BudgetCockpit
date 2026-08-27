#include "stdafx.h"
#include "BudgetManager.h"


// ============================================================
// TRANSAKTIONEN
// ============================================================

void BudgetManager::addTransaction(
    const Transaction& transaction)
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


// ============================================================
// BERECHNUNGEN
// ============================================================

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


// ============================================================
// FILTER: KATEGORIE
// ============================================================

QList<Transaction> BudgetManager::filterByCategory(
    const QString& category) const
{
    return filterByCategory(transactions, category);
}


QList<Transaction> BudgetManager::filterByCategory(
    const QList<Transaction>& transactionsToFilter,
    const QString& category) const
{
    const QString cleanedCategory = category.trimmed();

    // Eine leere Kategorie deaktiviert diesen Filter.
    if (cleanedCategory.isEmpty())
    {
        return transactionsToFilter;
    }

    QList<Transaction> filteredTransactions;

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


// ============================================================
// FILTER: TYP
// ============================================================

QList<Transaction> BudgetManager::filterByType(
    const QString& type) const
{
    return filterByType(transactions, type);
}


QList<Transaction> BudgetManager::filterByType(
    const QList<Transaction>& transactionsToFilter,
    const QString& type) const
{
    const QString cleanedType = type.trimmed();

    // Ein leerer Typ deaktiviert diesen Filter.
    if (cleanedType.isEmpty())
    {
        return transactionsToFilter;
    }

    QList<Transaction> filteredTransactions;

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


// ============================================================
// FILTER: ZEITRAUM
// ============================================================

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
    // Zwei ungültige QDate-Objekte bedeuten:
    // Der Datumsfilter ist deaktiviert.
    if (!fromDate.isValid() && !toDate.isValid())
    {
        return transactionsToFilter;
    }

    QList<Transaction> filteredTransactions;

    // Ein invertierter Zeitraum ist kein gültiger Filterbereich.
    if (fromDate.isValid()
        && toDate.isValid()
        && fromDate > toDate)
    {
        return filteredTransactions;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        const QDate transactionDate = transaction.getDate();

        const bool matchesFromDate =
            !fromDate.isValid()
            || transactionDate >= fromDate;

        const bool matchesToDate =
            !toDate.isValid()
            || transactionDate <= toDate;

        if (matchesFromDate && matchesToDate)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}


// ============================================================
// FILTER: BETRAG
// ============================================================

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
    // Ohne Unter- und Obergrenze ist der Betragsfilter deaktiviert.
    if (!minAmount.has_value() && !maxAmount.has_value())
    {
        return transactionsToFilter;
    }

    QList<Transaction> filteredTransactions;

    // Eine Untergrenze über der Obergrenze ist ungültig.
    if (minAmount.has_value()
        && maxAmount.has_value()
        && minAmount.value() > maxAmount.value())
    {
        return filteredTransactions;
    }

    for (const Transaction& transaction : transactionsToFilter)
    {
        const double amount = transaction.getAmount();

        const bool matchesMinimum =
            !minAmount.has_value()
            || amount >= minAmount.value();

        const bool matchesMaximum =
            !maxAmount.has_value()
            || amount <= maxAmount.value();

        if (matchesMinimum && matchesMaximum)
        {
            filteredTransactions.append(transaction);
        }
    }

    return filteredTransactions;
}


// ============================================================
// KOMBINIERTE FILTERUNG
// ============================================================

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