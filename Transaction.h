#pragma once

#include <QDate>
#include <QString>

class Transaction  // Klasse zur Darstellung einer Transaktion mit Datum, Typ, Kategorie, Betrag und Beschreibung
{
public:
    Transaction(
        const QDate& date,
        const QString& type,
        const QString& category,
        double amount,
        const QString& description
    );

    QDate getDate() const;
    QString getType() const;
    QString getCategory() const;
	double getAmount() const;  // Methode zum Abrufen des Betrags der Transaktion
	QString getDescription() const; // Methode zum Abrufen der Beschreibung der Transaktion

private:
    QDate date;
    QString type;
    QString category;
    double amount;
    QString description;
};