#include "stdafx.h"  // Vorabkompilierte Headerdatei
#include "BudgetCockpit.h"

BudgetCockpit::BudgetCockpit(QWidget *parent)  
    : QMainWindow(parent)
{
	ui.setupUi(this);  // Initialisierung der Benutzeroberfläche
}

BudgetCockpit::~BudgetCockpit() 
{}

