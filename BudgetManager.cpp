#include "BudgetManager.h"

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