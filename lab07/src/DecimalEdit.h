/*
 * DecimalEdit.h
 *
 *  Created on: 20 Feb 2020
 *      Author: DucVoo
 */

#ifndef DECIMALEDIT_H_
#define DECIMALEDIT_H_

#include "PropertyEdit.h"
#include "LiquidCrystal.h"
#include <string>

class DecimalEdit : public PropertyEdit
{
public:
    DecimalEdit(LiquidCrystal *lcd_, std::string editTitle, float step_, float min, float max);
    virtual ~DecimalEdit();
    void increment();
    void decrement();
    void accept();
    void cancel();
    void setFocus(bool focus);
    bool getFocus();
    void display();
    float getValue();
    float getEdit();
    float getUpper();
    float getLower();
    void setValue(float value);
    std::string getTitle() const;

private:
    void save();
    void displayEditValue();
    LiquidCrystal *lcd;
    std::string title;
    float value;
    float edit;
    bool focus;
    float upper;
    float lower;
    float step;
};

#endif /* DECIMALEDIT_H_ */
