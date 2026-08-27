#pragma once

#include <QString>
#include <QStringList>

class CategoryManager 
{
public:
    CategoryManager();

    bool addCategory(const QString& category);
    bool containsCategory(const QString& category) const; 

    const QStringList& getCategories() const;

private:
    QStringList categories;
};