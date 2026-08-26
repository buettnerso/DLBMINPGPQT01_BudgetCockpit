#include "stdafx.h"
#include "CategoryManager.h"

// Implementierung der Methoden der CategoryManager-Klasse

CategoryManager::CategoryManager()
{
    categories.append("Miete");
    categories.append("Lebensmittel");
    categories.append("Freizeit");
}

bool CategoryManager::addCategory(const QString& category)  // Methode zum Hinzufügen einer neuen Kategorie
{
    QString cleanedCategory = category.trimmed();

    if (cleanedCategory.isEmpty()) 
    {
        return false;
    }

	if (containsCategory(cleanedCategory)) // Überprüfen, ob die Kategorie bereits existiert
    {
        return false;
    }

	categories.append(cleanedCategory); // Hinzufügen der neuen Kategorie zur Liste
	return true; // Rückgabe von true, wenn die Kategorie erfolgreich hinzugefügt wurde
}

bool CategoryManager::containsCategory(const QString& category) const 
{
	QString cleanedCategory = category.trimmed(); // Entfernen von führenden und nachgestellten Leerzeichen

    for (const QString& existingCategory : categories) 
    {
        if (existingCategory.compare(
            cleanedCategory,
            Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }

	return false;  // Rückgabe von false, wenn die Kategorie nicht gefunden wurde
}

const QStringList& CategoryManager::getCategories() const  // Methode zum Abrufen der Liste der Kategorien
{
    return categories;
}