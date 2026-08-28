#include "stdafx.h"
#include "CsvRepository.h"

#include <QFile>
#include <QTextStream>
#include <QStringConverter>


bool CsvRepository::save(
    const QString& filePath,
    const QList<Transaction>& transactions,
    QString* errorMessage
) const
{
    QFile file(filePath);

    if (!file.open(
        QIODevice::WriteOnly |
        QIODevice::Text
    ))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei konnte nicht zum Schreiben geöffnet werden.";
        }

        return false;
    }


    QTextStream stream(&file);

    stream.setEncoding(
        QStringConverter::Utf8
    );


    // Kopfzeile
    stream
        << "Datum;"
        << "Art;"
        << "Kategorie;"
        << "Betrag;"
        << "Beschreibung\n";


    for (const Transaction& transaction : transactions)
    {
        stream
            << transaction.getDate()
            .toString("yyyy-MM-dd")
            << ";"

            << escapeCsvField(
                transaction.getType()
            )
            << ";"

            << escapeCsvField(
                transaction.getCategory()
            )
            << ";"

            << QString::number(
                transaction.getAmount(),
                'f',
                2
            )
            << ";"

            << escapeCsvField(
                transaction.getDescription()
            )
            << "\n";
    }


    file.close();

    return true;
}


bool CsvRepository::load(
    const QString& filePath,
    QList<Transaction>& transactions,
    QString* errorMessage
) const
{
    QFile file(filePath);

    if (!file.open(
        QIODevice::ReadOnly |
        QIODevice::Text
    ))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei konnte nicht geöffnet werden.";
        }

        return false;
    }


    QTextStream stream(&file);

    stream.setEncoding(
        QStringConverter::Utf8
    );


    // Erst in eine temporäre Liste laden.
    // Dadurch bleiben vorhandene Daten erhalten,
    // falls die CSV fehlerhaft ist.
    QList<Transaction> loadedTransactions;


    int lineNumber = 0;


    // Kopfzeile einlesen
    if (!stream.atEnd())
    {
        stream.readLine();
        ++lineNumber;
    }


    while (!stream.atEnd())
    {
        const QString line =
            stream.readLine();

        ++lineNumber;


        if (line.trimmed().isEmpty())
        {
            continue;
        }


        bool parseOk = false;

        const QStringList fields =
            parseCsvLine(
                line,
                &parseOk
            );


        if (!parseOk ||
            fields.size() != 5)
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Ungültiges CSV-Format in Zeile %1."
                    ).arg(lineNumber);
            }

            return false;
        }


        const QDate date =
            QDate::fromString(
                fields.at(0).trimmed(),
                "yyyy-MM-dd"
            );


        if (!date.isValid())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Ungültiges Datum in Zeile %1."
                    ).arg(lineNumber);
            }

            return false;
        }


        const QString type =
            fields.at(1).trimmed();

        const QString category =
            fields.at(2).trimmed();


        QString amountText =
            fields.at(3).trimmed();

        // Unterstützt zusätzlich deutsches Dezimaltrennzeichen,
        // falls die Datei beispielsweise in Excel bearbeitet wurde.
        amountText.replace(',', '.');


        bool amountOk = false;

        const double amount =
            amountText.toDouble(
                &amountOk
            );


        if (!amountOk)
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Ungültiger Betrag in Zeile %1."
                    ).arg(lineNumber);
            }

            return false;
        }


        const QString description =
            fields.at(4);


        loadedTransactions.append(
            Transaction(
                date,
                type,
                category,
                amount,
                description
            )
        );
    }


    file.close();


    // Erst jetzt die bisherige Liste ersetzen.
    transactions =
        loadedTransactions;


    return true;
}


QString CsvRepository::escapeCsvField(
    const QString& value
) const
{
    QString escaped = value;


    // Doppelte Anführungszeichen innerhalb
    // eines CSV-Feldes werden verdoppelt.
    escaped.replace(
        "\"",
        "\"\""
    );


    // Enthält das Feld ein Semikolon oder
    // Anführungszeichen, wird es vollständig
    // in Anführungszeichen gesetzt.
    if (escaped.contains(';') ||
        escaped.contains('"'))
    {
        escaped =
            "\"" + escaped + "\"";
    }


    return escaped;
}


QStringList CsvRepository::parseCsvLine(
    const QString& line,
    bool* ok
) const
{
    QStringList fields;

    QString currentField;

    bool insideQuotes = false;


    for (int i = 0;
        i < line.size();
        ++i)
    {
        const QChar character =
            line.at(i);


        if (character == '"')
        {
            // Zwei Anführungszeichen innerhalb
            // eines Textfeldes bedeuten ein
            // tatsächliches Anführungszeichen.
            if (insideQuotes &&
                i + 1 < line.size() &&
                line.at(i + 1) == '"')
            {
                currentField += '"';
                ++i;
            }
            else
            {
                insideQuotes =
                    !insideQuotes;
            }
        }
        else if (character == ';' &&
            !insideQuotes)
        {
            fields.append(
                currentField
            );

            currentField.clear();
        }
        else
        {
            currentField +=
                character;
        }
    }


    if (insideQuotes)
    {
        if (ok != nullptr)
        {
            *ok = false;
        }

        return {};
    }


    fields.append(
        currentField
    );


    if (ok != nullptr)
    {
        *ok = true;
    }


    return fields;
}