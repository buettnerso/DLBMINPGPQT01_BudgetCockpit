#include "stdafx.h"
#include "CategoryManager.h"


// ============================================================
// INITIALISIERUNG
// ============================================================

CategoryManager::CategoryManager()
{
    categories.append("Miete");
    categories.append("Lebensmittel");
    categories.append("Freizeit");
}


// ============================================================
// KATEGORIEN VERWALTEN
// ============================================================

bool CategoryManager::addCategory(const QString& category)
{
    const QString cleanedCategory = category.trimmed();

    if (cleanedCategory.isEmpty())
    {
        return false;
    }

    if (containsCategory(cleanedCategory))
    {
        return false;
    }

    categories.append(cleanedCategory);
    return true;
}


bool CategoryManager::containsCategory(const QString& category) const
{
    const QString cleanedCategory = category.trimmed();

    for (const QString& existingCategory : categories)
    {
        if (existingCategory.compare(
            cleanedCategory,
            Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }

    return false;
}


// ============================================================
// KATEGORIEN ABRUFEN
// ============================================================

const QStringList& CategoryManager::getCategories() const
{
    return categories;
}