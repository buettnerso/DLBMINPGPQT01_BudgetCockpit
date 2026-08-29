#include "stdafx.h"  // Vorabkompilierte Headerdatei
#include "BudgetCockpit.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLocale>

#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QLegend>
#include <QPainter>

#include <optional>

BudgetCockpit::BudgetCockpit(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    initializeGui();

    // Signal-Slot-Verbindungen
    connect(
        ui.btnAddTransaction,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::addTransaction
    );

    connect(
        ui.btnDeleteTransaction,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::deleteTransaction
    );

    
    // Filter im Reiter Buchungen

    connect(
        ui.btnApplyFilter,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::applyFilter
    );

    connect(
        ui.btnResetFilter,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::resetFilter
    );
           
    // Filter im Reiter Auswertung

    connect(
        ui.btnApplyAnalysisFilter,
        &QPushButton::clicked,
        this,
        [this]()
        {
            syncAnalysisFilterToBooking();
            applyFilter();
        }
    );

    connect(
        ui.btnResetAnalysisFilter,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::resetFilter
    );

    connect(
        ui.btnLoadCsv,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::openCsvFile
    );

    connect(
        ui.btnSaveCsv,
        &QPushButton::clicked,
        this,
        &BudgetCockpit::saveCsvFile
    );

   
}


BudgetCockpit::~BudgetCockpit()
{}

void BudgetCockpit::initializeGui()
{
    // Aktuelles Datum für neue Buchungen
    ui.dateTransaction->setDate(
        QDate::currentDate()
    );


    // --------------------------------------------------------
    // Betragsfilter Buchungen
    // --------------------------------------------------------

    ui.spnFilterAmountFrom->setSpecialValueText(
        "offen"
    );

    ui.spnFilterAmountTo->setSpecialValueText(
        "offen"
    );


    // --------------------------------------------------------
    // Betragsfilter Auswertung
    // --------------------------------------------------------

    ui.spnAnalysisAmountFrom->setSpecialValueText(
        "offen"
    );

    ui.spnAnalysisAmountTo->setSpecialValueText(
        "offen"
    );


    // Kategorien laden
    refreshCategories();


    // Auswertung übernimmt beim Start den Filterzustand aus Buchungen
    syncBookingFilterToAnalysis();
}

void BudgetCockpit::refreshCategories()
{
    // --------------------------------------------------------
    // Kategorieauswahl für neue Buchungen
    // --------------------------------------------------------

    ui.cmbCategory->clear();

    ui.cmbCategory->addItems(
        categoryManager.getCategories()
    );


    // --------------------------------------------------------
    // Kategorieauswahl für Filter im Reiter Buchungen
    // --------------------------------------------------------

    ui.cmbFilterCategory->clear();

    ui.cmbFilterCategory->addItem(
        "Alle Kategorien"
    );

    ui.cmbFilterCategory->addItems(
        categoryManager.getCategories()
    );


    // --------------------------------------------------------
    // Kategorieauswahl für Filter im Reiter Auswertung
    // --------------------------------------------------------

    ui.cmbAnalysisCategory->clear();

    ui.cmbAnalysisCategory->addItem(
        "Alle Kategorien"
    );

    ui.cmbAnalysisCategory->addItems(
        categoryManager.getCategories()
    );
}

//--------------------------------------------------------------
// Filter zwischen Buchungen und Auswertung synchronisieren
//--------------------------------------------------------------

void BudgetCockpit::syncBookingFilterToAnalysis()
{
    ui.dateAnalysisFrom->setDate(
        ui.dateFilterFrom->date()
    );

    ui.dateAnalysisTo->setDate(
        ui.dateFilterTo->date()
    );

    ui.spnAnalysisAmountFrom->setValue(
        ui.spnFilterAmountFrom->value()
    );

    ui.spnAnalysisAmountTo->setValue(
        ui.spnFilterAmountTo->value()
    );

    ui.cmbAnalysisCategory->setCurrentText(
        ui.cmbFilterCategory->currentText()
    );

    ui.cmbAnalysisType->setCurrentText(
        ui.cmbFilterType->currentText()
    );
}


void BudgetCockpit::syncAnalysisFilterToBooking()
{
    ui.dateFilterFrom->setDate(
        ui.dateAnalysisFrom->date()
    );

    ui.dateFilterTo->setDate(
        ui.dateAnalysisTo->date()
    );

    ui.spnFilterAmountFrom->setValue(
        ui.spnAnalysisAmountFrom->value()
    );

    ui.spnFilterAmountTo->setValue(
        ui.spnAnalysisAmountTo->value()
    );

    ui.cmbFilterCategory->setCurrentText(
        ui.cmbAnalysisCategory->currentText()
    );

    ui.cmbFilterType->setCurrentText(
        ui.cmbAnalysisType->currentText()
    );
}

//--------------------------------------------------------------
// Kreisdiagramm der gefilterten Buchungen aktualisieren
//--------------------------------------------------------------

void BudgetCockpit::refreshAnalysisChart(
    const QList<Transaction>& transactions)
{
    // --------------------------------------------------------
    // 1. Vorheriges Diagramm entfernen
    // --------------------------------------------------------

    if (analysisChartView != nullptr)
    {
        ui.chartContainerLayout->removeWidget(
            analysisChartView
        );

        delete analysisChartView;
        analysisChartView = nullptr;
    }


    // --------------------------------------------------------
    // 2. Überschrift passend zum Filter setzen
    // --------------------------------------------------------

    const QString selectedType =
        ui.cmbFilterType->currentText();


    if (selectedType == "Ausgabe")
    {
        ui.lblAnalysisTitle->setText(
            "Ausgaben nach Kategorie"
        );
    }
    else if (selectedType == "Einnahme")
    {
        ui.lblAnalysisTitle->setText(
            "Einnahmen nach Kategorie"
        );
    }
    else
    {
        ui.lblAnalysisTitle->setText(
            "Verteilung nach Kategorie"
        );
    }


    // --------------------------------------------------------
    // 3. Aktuellen Zeitraum anzeigen
    // --------------------------------------------------------

    ui.lblAnalysisPeriod->setText(
        QString(
            "%1 bis %2 | %3 Buchungen"
        )
        .arg(
            ui.dateFilterFrom
            ->date()
            .toString("dd.MM.yyyy")
        )
        .arg(
            ui.dateFilterTo
            ->date()
            .toString("dd.MM.yyyy")
        )
        .arg(
            transactions.size()
        )
    );


    // --------------------------------------------------------
    // 4. Keine Daten vorhanden
    // --------------------------------------------------------

    if (transactions.isEmpty())
    {
        ui.lblChartPlaceholder->setText(
            "Für die aktuelle Filterauswahl "
            "sind keine Buchungen vorhanden."
        );

        ui.lblChartPlaceholder->show();

        return;
    }


    // --------------------------------------------------------
    // 5. Beträge je Kategorie berechnen
    // --------------------------------------------------------

    const QMap<QString, double> categoryTotals =
        budgetManager.calculateCategoryTotals(
            transactions
        );


    // --------------------------------------------------------
    // 6. Kreisdiagramm-Serie erzeugen
    // --------------------------------------------------------

    QPieSeries* series =
        new QPieSeries();


    for (auto it = categoryTotals.cbegin();
        it != categoryTotals.cend();
        ++it)
    {
        // Kategorien ohne positiven Betrag
        // nicht als Segment darstellen.
        if (it.value() <= 0.0)
        {
            continue;
        }

        series->append(
            it.key(),
            it.value()
        );
    }


    // --------------------------------------------------------
    // 7. Prüfen, ob darstellbare Daten vorhanden sind
    // --------------------------------------------------------

    if (series->isEmpty())
    {
        delete series;

        ui.lblChartPlaceholder->setText(
            "Für die aktuelle Filterauswahl "
            "sind keine darstellbaren Werte vorhanden."
        );

        ui.lblChartPlaceholder->show();

        return;
    }


    // --------------------------------------------------------
    // 8. Prozentwerte an den Segmenten anzeigen
    // --------------------------------------------------------

    const QLocale germanLocale(
        QLocale::German,
        QLocale::Germany
    );


    for (QPieSlice* slice : series->slices())
    {
        const double percentage =
            slice->percentage() * 100.0;

        const QString category =
            slice->label();


        slice->setLabel(
            QString(
                "%1\n%2 %"
            )
            .arg(category)
            .arg(
                germanLocale.toString(
                    percentage,
                    'f',
                    1
                )
            )
        );


        slice->setLabelVisible(true);

        slice->setLabelPosition(
            QPieSlice::LabelOutside
        );
    }


    // --------------------------------------------------------
    // 9. Diagramm erzeugen
    // --------------------------------------------------------

    QChart* chart =
        new QChart();

    chart->addSeries(
        series
    );


    // Legende anzeigen
    chart->legend()->setVisible(true);

    chart->legend()->setAlignment(
        Qt::AlignRight
    );


    // Dezente Animation beim Aktualisieren
    chart->setAnimationOptions(
        QChart::SeriesAnimations
    );


    // --------------------------------------------------------
    // 10. ChartView erzeugen
    // --------------------------------------------------------

    analysisChartView =
        new QChartView(
            chart,
            ui.chartContainer
        );


    analysisChartView->setRenderHint(
        QPainter::Antialiasing
    );


    // Platzhalter ausblenden
    ui.lblChartPlaceholder->hide();


    // Diagramm in vorhandenes Layout einfügen
    ui.chartContainerLayout->addWidget(
        analysisChartView
    );
}

void BudgetCockpit::addTransaction()
{
    // --------------------------------------------------------
    // 1. Eingaben aus der GUI lesen
    // --------------------------------------------------------

    const QDate date =
        ui.dateTransaction->date();

    const QString type =
        ui.cmbTransactionType->currentText();

    const QString category =
        ui.cmbCategory->currentText();

    const double amount =
        ui.spnAmount->value();

    const QString description =
        ui.txtDescription->text().trimmed();


    // --------------------------------------------------------
    // 2. Eingaben prüfen
    // --------------------------------------------------------

    if (!date.isValid())
    {
        QMessageBox::warning(
            this,
            "Ungültige Eingabe",
            "Bitte wählen Sie ein gültiges Datum aus."
        );

        return;
    }


    if (category.trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            "Ungültige Eingabe",
            "Bitte wählen Sie eine Kategorie aus."
        );

        return;
    }


    if (amount <= 0.0)
    {
        QMessageBox::warning(
            this,
            "Ungültige Eingabe",
            "Bitte geben Sie einen Betrag größer als 0,00 € ein."
        );

        return;
    }


    // --------------------------------------------------------
    // 3. Falls noch keine CSV aktiv ist:
    //    Speicherort auswählen
    // --------------------------------------------------------

    if (currentCsvFilePath.isEmpty())
    {
        QString filePath =
            QFileDialog::getSaveFileName(
                this,
                "Budget-Datei erstellen",
                QString(),
                "CSV-Dateien (*.csv);;Alle Dateien (*.*)"
            );


        if (filePath.isEmpty())
        {
            return;
        }


        if (!filePath.endsWith(
            ".csv",
            Qt::CaseInsensitive))
        {
            filePath += ".csv";
        }


        currentCsvFilePath =
            filePath;
    }


    // --------------------------------------------------------
    // 4. Neue Transaction erzeugen
    // --------------------------------------------------------

    Transaction transaction(
        date,
        type,
        category,
        amount,
        description
    );


    // --------------------------------------------------------
    // 5. Buchung im BudgetManager speichern
    // --------------------------------------------------------

    budgetManager.addTransaction(
        transaction
    );


    // --------------------------------------------------------
    // 6. Aktive CSV automatisch speichern
    // --------------------------------------------------------

    if (!saveCurrentCsvFile())
    {
        return;
    }


    // --------------------------------------------------------
    // 7. Aktuelle Ansicht aktualisieren
    // --------------------------------------------------------

    if (filterActive)
    {
        // Bestehenden Filter beibehalten.
        applyFilter();
    }
    else
    {
        // Ohne aktiven Filter alle Buchungen anzeigen.
        resetFilter();
    }


    // --------------------------------------------------------
    // 8. Statusleiste aktualisieren
    // --------------------------------------------------------

    statusBar()->showMessage(
        "Gespeichert in: " +
        QFileInfo(
            currentCsvFilePath
        ).fileName(),
        4000
    );


    // --------------------------------------------------------
    // 9. Eingabefelder zurücksetzen
    // --------------------------------------------------------

    ui.spnAmount->setValue(0.0);

    ui.txtDescription->clear();

    ui.dateTransaction->setDate(
        QDate::currentDate()
    );
}


void BudgetCockpit::deleteTransaction()
{
    // --------------------------------------------------------
    // 1. Ausgewählte Tabellenzeile ermitteln
    // --------------------------------------------------------

    const int selectedRow =
        ui.tblTransactions->currentRow();


    if (selectedRow < 0 ||
        selectedRow >= displayedTransactions.size())
    {
        QMessageBox::warning(
            this,
            "Keine Buchung ausgewählt",
            "Bitte wählen Sie zuerst eine Buchung in der Tabelle aus."
        );

        return;
    }


    // --------------------------------------------------------
    // 2. Ausgewählte Buchung aus der aktuellen Ansicht holen
    // --------------------------------------------------------

    const Transaction selectedTransaction =
        displayedTransactions.at(selectedRow);


    // --------------------------------------------------------
    // 3. Sicherheitsabfrage
    // --------------------------------------------------------

    const QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Buchung löschen",
            QString(
                "Möchten Sie diese Buchung wirklich löschen?\n\n"
                "%1 | %2 | %3 | %4 €\n%5"
            )
            .arg(
                selectedTransaction
                .getDate()
                .toString("dd.MM.yyyy")
            )
            .arg(
                selectedTransaction.getType()
            )
            .arg(
                selectedTransaction.getCategory()
            )
            .arg(
                QString::number(
                    selectedTransaction.getAmount(),
                    'f',
                    2
                )
            )
            .arg(
                selectedTransaction.getDescription()
            ),
            QMessageBox::Yes |
            QMessageBox::No,
            QMessageBox::No
        );


    if (answer != QMessageBox::Yes)
    {
        return;
    }


    // --------------------------------------------------------
    // 4. Passende Buchung im vollständigen Datenbestand suchen
    // --------------------------------------------------------

    const QList<Transaction>& allTransactions =
        budgetManager.getAllTransactions();

    int managerIndex = -1;


    for (int i = 0;
        i < allTransactions.size();
        ++i)
    {
        const Transaction& transaction =
            allTransactions.at(i);


        const bool sameTransaction =
            transaction.getDate()
            == selectedTransaction.getDate()
            &&
            transaction.getType()
            == selectedTransaction.getType()
            &&
            transaction.getCategory()
            == selectedTransaction.getCategory()
            &&
            transaction.getAmount()
            == selectedTransaction.getAmount()
            &&
            transaction.getDescription()
            == selectedTransaction.getDescription();


        if (sameTransaction)
        {
            managerIndex = i;
            break;
        }
    }


    if (managerIndex < 0)
    {
        QMessageBox::critical(
            this,
            "Löschen fehlgeschlagen",
            "Die ausgewählte Buchung konnte im Datenbestand nicht gefunden werden."
        );

        return;
    }


    // --------------------------------------------------------
    // 5. Buchung aus dem BudgetManager entfernen
    // --------------------------------------------------------

    if (!budgetManager.removeTransaction(
        managerIndex))
    {
        QMessageBox::critical(
            this,
            "Löschen fehlgeschlagen",
            "Die Buchung konnte nicht gelöscht werden."
        );

        return;
    }


    // --------------------------------------------------------
    // 6. Aktuelle CSV automatisch aktualisieren
    // --------------------------------------------------------

    if (!currentCsvFilePath.isEmpty())
    {
        if (!saveCurrentCsvFile())
        {
            return;
        }
    }


    // --------------------------------------------------------
    // 7. GUI aktualisieren
    // --------------------------------------------------------

    if (filterActive)
    {
        // Bestehenden Filter erneut anwenden.
        applyFilter();
    }
    else
    {
        // Ohne aktiven Filter alle Buchungen anzeigen.
        resetFilter();
    }


    // --------------------------------------------------------
    // 8. Rückmeldung
    // --------------------------------------------------------

    statusBar()->showMessage(
        "Buchung gelöscht und gespeichert.",
        4000
    );
}


void BudgetCockpit::applyFilter()
{
    // --------------------------------------------------------
    // 1. Kategorie auslesen
    // --------------------------------------------------------

    QString category =
        ui.cmbFilterCategory->currentText();

    if (category == "Alle Kategorien")
    {
        category.clear();
    }


    // --------------------------------------------------------
    // 2. Typ auslesen
    // --------------------------------------------------------

    QString type =
        ui.cmbFilterType->currentText();

    if (type == "Alle")
    {
        type.clear();
    }


    // --------------------------------------------------------
    // 3. Zeitraum auslesen
    // --------------------------------------------------------

    const QDate fromDate =
        ui.dateFilterFrom->date();

    const QDate toDate =
        ui.dateFilterTo->date();


    if (fromDate > toDate)
    {
        QMessageBox::warning(
            this,
            "Ungültiger Zeitraum",
            "Das Startdatum darf nicht nach dem Enddatum liegen."
        );

        return;
    }


    // --------------------------------------------------------
    // 4. Betragsgrenzen auslesen
    // --------------------------------------------------------

    std::optional<double> minAmount;
    std::optional<double> maxAmount;


    const double minValue =
        ui.spnFilterAmountFrom->value();

    const double maxValue =
        ui.spnFilterAmountTo->value();


    // 0,00 bedeutet:
    // Für diese Seite ist keine Betragsgrenze gesetzt.
    if (minValue > 0.0)
    {
        minAmount = minValue;
    }

    if (maxValue > 0.0)
    {
        maxAmount = maxValue;
    }


    if (minAmount.has_value()
        && maxAmount.has_value()
        && minAmount.value() > maxAmount.value())
    {
        QMessageBox::warning(
            this,
            "Ungültiger Betrag",
            "Der Mindestbetrag darf nicht größer als der Höchstbetrag sein."
        );

        return;
    }


    // --------------------------------------------------------
    // 5. Filter über BudgetManager anwenden
    // --------------------------------------------------------

    displayedTransactions =
        budgetManager.filterTransactions(
            category,
            type,
            fromDate,
            toDate,
            minAmount,
            maxAmount
        );


    // --------------------------------------------------------
    // 6. Tabelle aktualisieren
    // --------------------------------------------------------

    refreshTransactionTable(
        displayedTransactions
    );


    // --------------------------------------------------------
    // 7. Kennzahlen für die gefilterten Daten aktualisieren
    // --------------------------------------------------------

    refreshStatistics(
        displayedTransactions
    );

	// Kreisdiagramm für die gefilterten Daten aktualisieren
    refreshAnalysisChart(
        displayedTransactions
    );


    // Filter ist ab jetzt aktiv.
    filterActive = true;

    // Aktiven Filter auch im Reiter Auswertung anzeigen.
    syncBookingFilterToAnalysis();


    // --------------------------------------------------------
    // 8. Status anzeigen
    // --------------------------------------------------------

    statusBar()->showMessage(
        QString(
            "Filter angewendet: %1 Buchungen"
        )
        .arg(displayedTransactions.size()),
        4000
    );
}


void BudgetCockpit::resetFilter()
{
    // Filter ist ab jetzt deaktiviert.
    filterActive = false;

    // --------------------------------------------------------
    // 1. Kategorie zurücksetzen
    // --------------------------------------------------------

    ui.cmbFilterCategory->setCurrentIndex(0);


    // --------------------------------------------------------
    // 2. Typ zurücksetzen
    // --------------------------------------------------------

    ui.cmbFilterType->setCurrentIndex(0);


    // --------------------------------------------------------
    // 3. Betragsfilter zurücksetzen
    // --------------------------------------------------------

    ui.spnFilterAmountFrom->setValue(0.0);
    ui.spnFilterAmountTo->setValue(0.0);


    // --------------------------------------------------------
    // 4. Datumsbereich auf gesamten Datenbestand setzen
    // --------------------------------------------------------

    const QList<Transaction>& allTransactions =
        budgetManager.getAllTransactions();


    if (!allTransactions.isEmpty())
    {
        QDate earliestDate =
            allTransactions.first().getDate();

        QDate latestDate =
            allTransactions.first().getDate();


        for (const Transaction& transaction :
            allTransactions)
        {
            if (transaction.getDate() < earliestDate)
            {
                earliestDate =
                    transaction.getDate();
            }

            if (transaction.getDate() > latestDate)
            {
                latestDate =
                    transaction.getDate();
            }
        }


        ui.dateFilterFrom->setDate(
            earliestDate
        );

        ui.dateFilterTo->setDate(
            latestDate
        );
    }
    else
    {
        ui.dateFilterFrom->setDate(
            QDate::currentDate()
        );

        ui.dateFilterTo->setDate(
            QDate::currentDate()
        );
    }


    // --------------------------------------------------------
    // 5. Wieder alle Buchungen anzeigen
    // --------------------------------------------------------

    displayedTransactions =
        allTransactions;


    refreshTransactionTable(
        displayedTransactions
    );


    refreshStatistics(
        displayedTransactions
    );

	// Kreisdiagramm für die gefilterten Daten aktualisieren
    refreshAnalysisChart(
        displayedTransactions
    );

    // --------------------------------------------------------
    // 6. Zurückgesetzten Filter mit Auswertung synchronisieren
    // --------------------------------------------------------

    syncBookingFilterToAnalysis();


    statusBar()->showMessage(
        "Filter zurückgesetzt.",
        3000
    );
}

//--------------------------------------------------------------
// CSV FUnktionen
//--------------------------------------------------------------

void BudgetCockpit::openCsvFile()
{
    const QString filePath =
        QFileDialog::getOpenFileName(
            this,
            "Budget-Datei öffnen",
            QString(),
            "CSV-Dateien (*.csv);;Alle Dateien (*.*)"
        );


    // Abbrechen wurde geklickt
    if (filePath.isEmpty())
    {
        return;
    }


    QList<Transaction> loadedTransactions;
    QString errorMessage;


    const bool success =
        csvRepository.load(
            filePath,
            loadedTransactions,
            &errorMessage
        );


    if (!success)
    {
        QMessageBox::critical(
            this,
            "CSV-Datei konnte nicht geladen werden",
            errorMessage
        );

        return;
    }


    // Geladene Daten werden zum neuen
    // Datenbestand des BudgetManagers.
    budgetManager.setTransactions(
        loadedTransactions
    );


    // Diese CSV ist ab jetzt die aktive Arbeitsdatei.
    currentCsvFilePath =
        filePath;


    // Aktuelle Ansicht aktualisieren.
    displayedTransactions =
        loadedTransactions;


    // Kategorien aus der geladenen CSV übernehmen.
    for (const Transaction& transaction :
        loadedTransactions)
    {
        categoryManager.addCategory(
            transaction.getCategory()
        );
    }


    // Kategorie-ComboBoxen aktualisieren.
    refreshCategories();

    // Filter und Ansicht auf den neu geladenen
    // Datenbestand zurücksetzen.
    resetFilter();


    // Aktuell aktive Datei in der Statusleiste anzeigen.
    statusBar()->showMessage(
        "Aktive Budget-Datei: " +
        QFileInfo(filePath).fileName()
    );


    // Erst jetzt Erfolgsmeldung anzeigen.
    QMessageBox::information(
        this,
        "Budget-Datei geladen",
        QString(
            "%1 Buchungen wurden aus\n%2\ngeladen."
        )
        .arg(loadedTransactions.size())
        .arg(QFileInfo(filePath).fileName())
    );
}

//--------------------------------------------------------------
// CSV Speichern unter ...
//--------------------------------------------------------------

void BudgetCockpit::saveCsvFile()
{
    QString suggestedPath =
        currentCsvFilePath;

    const QString filePath =
        QFileDialog::getSaveFileName(
            this,
            "Budget-Datei speichern",
            suggestedPath,
            "CSV-Dateien (*.csv);;Alle Dateien (*.*)"
        );

    // Nutzer hat Abbrechen gewählt.
    if (filePath.isEmpty())
    {
        return;
    }

    QString finalFilePath =
        filePath;

    // Falls keine Dateiendung angegeben wurde,
    // automatisch .csv ergänzen.
    if (!finalFilePath.endsWith(
        ".csv",
        Qt::CaseInsensitive))
    {
        finalFilePath += ".csv";
    }

    QString errorMessage;

    const bool success =
        csvRepository.save(
            finalFilePath,
            budgetManager.getAllTransactions(),
            &errorMessage
        );

    if (!success)
    {
        QMessageBox::critical(
            this,
            "Budget-Datei konnte nicht gespeichert werden",
            errorMessage
        );

        return;
    }

    // Die gespeicherte Datei wird zur
    // aktuellen Arbeitsdatei.
    currentCsvFilePath =
        finalFilePath;

    // Aktive Datei anzeigen.
    statusBar()->showMessage(
        "Aktive Budget-Datei: " +
        QFileInfo(currentCsvFilePath).fileName()
    );

    QMessageBox::information(
        this,
        "Budget-Datei gespeichert",
        QString(
            "%1 Buchungen wurden in\n%2\ngespeichert."
        )
        .arg(
            budgetManager
            .getAllTransactions()
            .size()
        )
        .arg(
            QFileInfo(
                currentCsvFilePath
            ).fileName()
        )
    );
}


//--------------------------------------------------------------
// Aktuelle CSV automatisch speichern
//--------------------------------------------------------------

bool BudgetCockpit::saveCurrentCsvFile()
{
    // Ohne aktive CSV kann nicht gespeichert werden.
    if (currentCsvFilePath.isEmpty())
    {
        return false;
    }

    QString errorMessage;

    const bool success =
        csvRepository.save(
            currentCsvFilePath,
            budgetManager.getAllTransactions(),
            &errorMessage
        );

    if (!success)
    {
        QMessageBox::critical(
            this,
            "Speichern fehlgeschlagen",
            errorMessage
        );

        return false;
    }

    return true;
}
