/*
 * IntegerEdit.cpp
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#include "IntegerEdit.h"
#include <cstdio>

IntegerEdit::IntegerEdit(LiquidCrystal *lcd_, std::string editTitle, int min, int max) : lcd(lcd_), title(editTitle), lower(min), upper(max)
{
	updatedValue = nullptr;
	step = 1;
	value = 0;
	edit = 0;
	focus = false;
}

IntegerEdit::IntegerEdit(LiquidCrystal *lcd_, std::string editTitle, int step_, int min, int max) : lcd(lcd_), title(editTitle), step(step_), lower(min), upper(max)
{
	updatedValue = nullptr;
	value = 0;
	edit = 0;
	focus = false;
}

IntegerEdit::~IntegerEdit()
{
}

void IntegerEdit::increment()
{
	if (edit < upper)
		edit += step;
	if (edit >= upper)
		edit = upper;
}

void IntegerEdit::decrement()
{
	if (edit > lower)
		edit -= step;
	if (edit <= lower)
		edit = lower;
}

void IntegerEdit::accept()
{
	onState = true;
	save();
}

void IntegerEdit::cancel()
{
	edit = value;
}

void IntegerEdit::setFocus(bool focus)
{
	this->focus = focus;
}

bool IntegerEdit::getFocus()
{
	return this->focus;
}

void IntegerEdit::display()
{
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->print(title);
	lcd->setCursor(0, 1);
	char s[17];
	if (focus)
	{
		snprintf(s, 17, "     [%4d]     ", edit);
	}
	else
	{
		snprintf(s, 17, "      %4d      ", edit);
	}
	lcd->print(s);
}

void IntegerEdit::save()
{

	if (value != edit)
	{
		// set current value to be same as edit value
		value = edit;
		saveValue = true;
	}
	else
	{
		saveValue = false;
	}
	if (updatedValue != nullptr)
		updatedValue();
	// todo: save current value for example to EEPROM for permanent storage
}

int IntegerEdit::getValue()
{
	return value;
}

int IntegerEdit::getEdit()
{
	return edit;
}
int IntegerEdit::getUpper()
{
	return upper;
}
int IntegerEdit::getLower()
{
	return lower;
}
std::string IntegerEdit::getTitle() const
{
	return title;
}
void IntegerEdit::setValue(int value)
{
	edit = value;
	this->value = edit;
	save();
}
