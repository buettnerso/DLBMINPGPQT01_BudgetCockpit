#pragma once

#include <QDate>
#include <QString>

class Transaction 
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
	double getAmount() const;
	QString getDescription() const;

private:
    QDate date;
    QString type;
    QString category;
    double amount;
    QString description;
};