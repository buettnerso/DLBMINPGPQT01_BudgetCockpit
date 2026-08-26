#pragma once

#include <QList>
#include "Transaction.h"

class BudgetManager
{
public:
    void addTransaction(const Transaction& transaction);
    bool removeTransaction(int index);

    const QList<Transaction>& getAllTransactions() const;

private:
    QList<Transaction> transactions;
};