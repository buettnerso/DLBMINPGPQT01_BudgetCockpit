#pragma once

#include <QList>
#include <QString>
#include <QStringList>

#include "Transaction.h"


class CsvRepository
{
public:
    CsvRepository() = default;

    bool save(
        const QString& filePath,
        const QList<Transaction>& transactions,
        QString* errorMessage = nullptr
    ) const;

    bool load(
        const QString& filePath,
        QList<Transaction>& transactions,
        QString* errorMessage = nullptr
    ) const;


private:
    QString escapeCsvField(
        const QString& value
    ) const;

    QStringList parseCsvLine(
        const QString& line,
        bool* ok
    ) const;
};