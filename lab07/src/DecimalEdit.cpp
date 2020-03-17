/*
 * DecimalEdit.cpp
 *
 *  Created on: 20 Feb 2020
 *      Author: DucVoo
 */

#include "DecimalEdit.h"
#include <cstdio>

DecimalEdit::DecimalEdit(LiquidCrystal *lcd_, std::string editTitle, float step_, float min, float max) : lcd(lcd_), title(editTitle), step(step_), lower(min), upper(max)
{
    value = 0;
    edit = 0;
    focus = false;
}

DecimalEdit::~DecimalEdit()
{
}

void DecimalEdit ::increment()
{
    if (edit < upper)
        edit += step;
    if (edit >= upper)
        edit = upper;
}

void DecimalEdit ::decrement()
{
    if (edit > lower)
        edit -= step;
    if (edit <= lower)
        edit = lower;
}

void DecimalEdit ::accept()
{
    onState = true;
    save();
}

void DecimalEdit ::cancel()
{
    edit = value;
}

void DecimalEdit ::setFocus(bool focus)
{
    this->focus = focus;
}

bool DecimalEdit ::getFocus()
{
    return this->focus;
}

void DecimalEdit ::display()
{
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(title);
    lcd->setCursor(0, 1);
    char s[17];
    if (focus)
    {
        snprintf(s, 17, "     [%4.2f]     ", edit);
    }
    else
    {
        snprintf(s, 17, "      %4.2f      ", edit);
    }
    lcd->print(s);
}

void DecimalEdit ::save()
{
    // set current value to be same as edit value
    value = edit;
    // todo: save current value for example to EEPROM for permanent storage
}

float DecimalEdit::getValue()
{
    return value;
}

float DecimalEdit::getEdit()
{
    return edit;
}
float DecimalEdit::getUpper()
{
    return upper;
}
float DecimalEdit::getLower()
{
    return lower;
}
std::string DecimalEdit::getTitle() const
{
    return title;
}
void DecimalEdit ::setValue(float value)
{
    edit = value;
    save();
}
