#include "stdafx.h"
#include "CsvRepository.h"

#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QStringConverter>

#include <cmath>


// ============================================================
// CSV speichern
// ============================================================

bool CsvRepository::save(
    const QString& filePath,
    const QList<Transaction>& transactions,
    QString* errorMessage
) const
{
    // QSaveFile schreibt zunächst in eine temporäre Datei.
    // Erst commit() ersetzt die Zieldatei, wenn das Schreiben
    // vollständig erfolgreich war.
    QSaveFile file(filePath);

    if (!file.open(
        QIODevice::WriteOnly |
        QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei konnte nicht zum Schreiben geöffnet werden.\n" +
                file.errorString();
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


    // Buchungen zeilenweise schreiben.
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


    // Gepufferte Daten vollständig an QSaveFile übergeben.
    stream.flush();

    if (stream.status() != QTextStream::Ok)
    {
        file.cancelWriting();

        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Beim Schreiben der CSV-Datei ist ein Fehler aufgetreten.\n" +
                file.errorString();
        }

        return false;
    }


    // Die temporäre Datei ersetzt erst jetzt die Zieldatei.
    if (!file.commit())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei konnte nicht sicher gespeichert werden.\n" +
                file.errorString();
        }

        return false;
    }


    return true;
}


// ============================================================
// CSV laden
// ============================================================

bool CsvRepository::load(
    const QString& filePath,
    QList<Transaction>& transactions,
    QString* errorMessage
) const
{
    QFile file(filePath);

    if (!file.open(
        QIODevice::ReadOnly |
        QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei konnte nicht geöffnet werden.\n" +
                file.errorString();
        }

        return false;
    }


    QTextStream stream(&file);

    stream.setEncoding(
        QStringConverter::Utf8
    );


    // Zunächst in eine temporäre Liste laden.
    // Der vorhandene Datenbestand wird nur bei vollständig
    // erfolgreichem Einlesen ersetzt.
    QList<Transaction> loadedTransactions;

    int lineNumber = 0;


    // --------------------------------------------------------
    // Kopfzeile prüfen
    // --------------------------------------------------------

    if (stream.atEnd())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei ist leer und enthält keine gültige Kopfzeile.";
        }

        return false;
    }


    const QString headerLine =
        stream.readLine();

    ++lineNumber;


    bool headerOk = false;

    const QStringList headerFields =
        parseCsvLine(
            headerLine,
            &headerOk
        );


    const QStringList expectedHeader =
    {
        "Datum",
        "Art",
        "Kategorie",
        "Betrag",
        "Beschreibung"
    };


    QStringList normalizedHeader;

    for (const QString& field : headerFields)
    {
        normalizedHeader.append(
            field.trimmed()
        );
    }


    if (!headerOk ||
        normalizedHeader != expectedHeader)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Die CSV-Datei besitzt keine gültige Kopfzeile.\n"
                "Erwartet wird: Datum;Art;Kategorie;Betrag;Beschreibung";
        }

        return false;
    }


    // --------------------------------------------------------
    // Buchungszeilen einlesen und validieren
    // --------------------------------------------------------

    while (!stream.atEnd())
    {
        const QString line =
            stream.readLine();

        ++lineNumber;


        // Leere Zeilen werden toleriert.
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


        // Datum
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


        // Art
        const QString type =
            fields.at(1).trimmed();


        if (type != "Einnahme" &&
            type != "Ausgabe")
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Ungültige Buchungsart in Zeile %1. "
                        "Erlaubt sind \"Einnahme\" und \"Ausgabe\"."
                    ).arg(lineNumber);
            }

            return false;
        }


        // Kategorie
        const QString category =
            fields.at(2).trimmed();


        if (category.isEmpty())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Die Kategorie in Zeile %1 darf nicht leer sein."
                    ).arg(lineNumber);
            }

            return false;
        }


        // Betrag
        QString amountText =
            fields.at(3).trimmed();

        // Zusätzlich deutsches Dezimaltrennzeichen akzeptieren,
        // z. B. nach einer manuellen Bearbeitung in Excel.
        amountText.replace(',', '.');


        bool amountOk = false;

        const double amount =
            amountText.toDouble(
                &amountOk
            );


        if (!amountOk ||
            !std::isfinite(amount) ||
            amount <= 0.0)
        {
            if (errorMessage != nullptr)
            {
                *errorMessage =
                    QString(
                        "Ungültiger Betrag in Zeile %1. "
                        "Der Betrag muss größer als 0,00 sein."
                    ).arg(lineNumber);
            }

            return false;
        }


        // Beschreibung ist optional.
        const QString description =
            fields.at(4).trimmed();


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


    if (stream.status() != QTextStream::Ok)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage =
                "Beim Lesen der CSV-Datei ist ein Fehler aufgetreten.\n" +
                file.errorString();
        }

        return false;
    }


    file.close();


    // Erst nach vollständiger Validierung wird der bisherige
    // Datenbestand durch die geladenen Buchungen ersetzt.
    transactions =
        loadedTransactions;


    return true;
}


// ============================================================
// CSV-Feld für die Ausgabe vorbereiten
// ============================================================

QString CsvRepository::escapeCsvField(
    const QString& value
) const
{
    QString escaped = value;


    // Anführungszeichen innerhalb eines Feldes verdoppeln.
    escaped.replace(
        "\"",
        "\"\""
    );


    // Felder mit Semikolon oder Anführungszeichen werden
    // vollständig in Anführungszeichen eingeschlossen.
    if (escaped.contains(';') ||
        escaped.contains('"'))
    {
        escaped =
            "\"" + escaped + "\"";
    }


    return escaped;
}


// ============================================================
// Eine CSV-Zeile in einzelne Felder zerlegen
// ============================================================

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
            // Zwei Anführungszeichen innerhalb eines
            // Textfeldes entsprechen einem echten ".
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


    // Nicht geschlossenes Anführungszeichen:
    // Zeile ist syntaktisch ungültig.
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
