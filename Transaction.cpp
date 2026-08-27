#include "stdafx.h"
#include "Transaction.h"


// ============================================================
// INITIALISIERUNG
// ============================================================

Transaction::Transaction(
    const QDate& date,
    const QString& type,
    const QString& category,
    double amount,
    const QString& description
)
    : date(date),
    type(type),
    category(category),
    amount(amount),
    description(description)
{}


// ============================================================
// EIGENSCHAFTEN ABRUFEN
// ============================================================

QDate Transaction::getDate() const
{
    return date;
}


QString Transaction::getType() const
{
    return type;
}


QString Transaction::getCategory() const
{
    return category;
}


double Transaction::getAmount() const
{
    return amount;
}


QString Transaction::getDescription() const
{
    return description;
}