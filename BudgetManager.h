#pragma once

#include <QList>
#include "Transaction.h"

// Klasse zur Verwaltung von Transaktionen und Berechnung von Einnahmen, Ausgaben und Saldo

class BudgetManager
{
public:
    void addTransaction(const Transaction& transaction);
    bool removeTransaction(int index);

    const QList<Transaction>& getAllTransactions() const;

    double calculateIncome() const;
    double calculateExpenses() const;
    double calculateBalance() const;

	double calculateIncome(const QList<Transaction>& transactionsToCalculate) const; // Berechnung des Einkommens basierend auf einer Liste von Transaktionen
	double calculateExpenses(const QList<Transaction>& transactionsToCalculate) const; // Berechnung der Ausgaben basierend auf
	double calculateBalance(const QList<Transaction>& transactionsToCalculate) const; // Berechnung des Saldos basierend auf einer Liste von Transaktionen

private:
    QList<Transaction> transactions;
};