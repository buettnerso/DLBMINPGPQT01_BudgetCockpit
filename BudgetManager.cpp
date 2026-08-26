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