#pragma once

#include <QList>
#include <QString>
#include <QStringList>

#include "Transaction.h"


class CsvRepository
{
public:
    CsvRepository() = default;


    // Vollständigen Datenbestand sicher als CSV speichern.
    bool save(
        const QString& filePath,
        const QList<Transaction>& transactions,
        QString* errorMessage = nullptr
    ) const;


    // CSV vollständig einlesen und validieren.
    // Bei einem Fehler bleibt die übergebene Liste unverändert.
    bool load(
        const QString& filePath,
        QList<Transaction>& transactions,
        QString* errorMessage = nullptr
    ) const;


private:
    // Sonderzeichen für ein semikolongetrenntes CSV-Feld maskieren.
    QString escapeCsvField(
        const QString& value
    ) const;


    // Eine CSV-Zeile unter Berücksichtigung von Anführungszeichen parsen.
    QStringList parseCsvLine(
        const QString& line,
        bool* ok
    ) const;
};
