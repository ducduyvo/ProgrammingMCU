/*
 * IntegerEdit.h
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#ifndef INTEGEREDIT_H_
#define INTEGEREDIT_H_

#include "PropertyEdit.h"
#include "LiquidCrystal.h"
#include <string>

class IntegerEdit : public PropertyEdit
{
public:
	IntegerEdit(LiquidCrystal *lcd_, std::string editTitle, int min, int max);
	IntegerEdit(LiquidCrystal *lcd_, std::string editTitle, int step_, int min, int max);
	virtual ~IntegerEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus();
	void display();
	int getValue();
	int getEdit();
	int getUpper();
	int getLower();
	void setValue(int value);
	std::string getTitle() const;
	void (*updatedValue) ();
	static bool saveValue; 


private:
	void save();
	void displayEditValue();
	LiquidCrystal *lcd;
	std::string title;
	int value;
	int edit;
	bool focus;
	int upper;
	int lower;
	int step;
};

#endif /* INTEGEREDIT_H_ */
