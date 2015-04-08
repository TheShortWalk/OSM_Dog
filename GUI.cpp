/*
 * GUI.cpp
 *
 * Created: 3/13/2015 12:21:51 AM
 *  Author: Pooter
 */ 

#define length(name) sizeof(name)/sizeof((name)[0])

#define F_CPU 16000000
//#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "GUI.h"
#include "LCD_8544.h"
#include "HID_Components.h"


LCD_8544 Display;

int16_t testValue = 0;

void set_testValue(){
	testValue++;
	//if(testValue < 0) testValue = 0;
	//else if(testValue > 50) testValue = 50;
};

int16_t get_testValue(){
	return testValue;
};



//______________________ GENERATE MENU  ______________________________

MenuItem_obj Items_MainMenu[] = {
	MenuItem_obj("Mode:"),
	MenuItem_obj("RUN"),
	MenuItem_obj("RETURN"),
	MenuItem_obj("Program", PROGRAM),
	MenuItem_obj("Settings", SETTINGS),
	MenuItem_obj("About", ABOUT),
	MenuItem_obj("Extra1", set_testValue),
	MenuItem_obj("Haii", to_char(get_testValue()))
};

MenuItem_obj Items_ProgramMenu[] = {
	MenuItem_obj("Back", MAIN)
};

MenuItem_obj Items_SettingsMenu[] = {
	MenuItem_obj("Back", MAIN)
};

MenuItem_obj Items_AboutMenu[] = {
	MenuItem_obj("Rev 2.0"),
	MenuItem_obj("Back", MAIN)
};

MenuPage_obj MenuPages[] = {
	MenuPage_obj(Items_MainMenu, length(Items_MainMenu)),
	MenuPage_obj(Items_ProgramMenu, length(Items_ProgramMenu)),
	MenuPage_obj(Items_SettingsMenu, length(Items_SettingsMenu)),
	MenuPage_obj(Items_AboutMenu, length(Items_AboutMenu))
};

GUI_obj GUI(MenuPages, length(MenuPages));

//______________________ MENU ITEMS __________________________________
MenuItem_obj::MenuItem_obj(){};

MenuItem_obj::MenuItem_obj(char *button_label){
	this->Label = button_label;
	this->type = TEXT;
};

//Function
MenuItem_obj::MenuItem_obj(char *button_label, void (*function)()){
	this->Label = button_label;
	this->buttonFunction = function;
	this->type = FUNCTION;
};

//MenuChange
MenuItem_obj::MenuItem_obj(char *button_label, MenuPageList menuPage){
	this->Label = button_label;
	this->menuLink = menuPage;
	this->type = MENUCHANGE;
}

MenuItem_obj::MenuItem_obj(char *button_label, char (*get)()){
	this->Label = button_label;
	this->getData = get;
	this->type = FIELD;
}

void MenuItem_obj::Draw(){
	Display.Write(Label);
	/*
	//Display.Write(Label);
	switch(Type){
		case BUTTON:
			//write nothing
			break;
		case LIST:
			//Display.Write() //write selected item
			break;
		case INCREMENT:
			//Display.Write() //write selected number
			break;
		default:
			break; //write nothing
		
	}
	//Display.Write();
	*/
};

void MenuItem_obj::itemSelect(){
	switch(type){
		case TEXT:
			break;
		case FUNCTION:
			buttonFunction();
			break;
		case FIELD:
			break;
		case MENUCHANGE:
			GUI.setMenuPosition(menuLink); 
			break;
		default:
			break;
	}
};

void MenuItem_obj::itemDeselect(){
	
};

//___________________________ MENU PAGES _______________________________

MenuPage_obj::MenuPage_obj(MenuItem_obj *Items, uint8_t listLength){
	this->MenuItem = Items;
	this->numberOfItems = listLength;
	this->CursorPosition = 0;
};

//Draws the menu to the LCD
void MenuPage_obj::Draw(){
	Display.Clear();
	uint8_t drawLine = 0;
	//Draw menu items until off screen or end of menu
	while (((DrawPosition + drawLine) < numberOfItems) && (drawLine < LCD_ROWS))
	{
		Display.gotoXY(0, drawLine);
		//set font to inverted when drawing menu item at CursorPosition
		if (drawLine == (CursorPosition - DrawPosition)) Display.setStyle(INVERT);
		MenuItem[DrawPosition + drawLine].Draw();
		Display.setStyle(NONE);
		drawLine++;
	}
	//help debug LCD menu scrolling
	//Display.gotoXY(48,0), Display.Write(CursorPosition);
	//Display.gotoXY(48,1), Display.Write(DrawPosition);
};

//handles scrolling of the menu
void MenuPage_obj::setCursorPosition(int8_t scrollValue){
	//Prevent cursor from scrolling off menu
	CursorPosition += scrollValue;
	if(CursorPosition < 0) CursorPosition = 0;
	else if(CursorPosition >= numberOfItems) CursorPosition = numberOfItems - 1;
	
	//Shifts the menu if scrolled off screen
	int8_t cursorOffset = CursorPosition - DrawPosition;
	if(cursorOffset >= LCD_ROWS) DrawPosition = CursorPosition - (LCD_ROWS - 1);
	else if(cursorOffset < 0) DrawPosition = CursorPosition;
};


//______________________________ GUI _____________________________

GUI_obj::GUI_obj(){};

GUI_obj::GUI_obj(MenuPage_obj *Pages, uint8_t listLength){
	this->MenuPage = Pages;
	this->numberOfMenus = listLength;
	
	this->Current_Page = MAIN;
	};
	
	
void GUI_obj::Begin(){
	Display.Begin();
	Display.Clear();
	this->DrawScreen();
	HID_Dial.Begin();
	HID_Button.Begin();
};

void GUI_obj::Update(){
	if(HID_Changed){
		HID_Changed = false;
		if(HID_Button.Changed) Handle_Button(HID_Button.state);
		if(HID_Dial.Changed) Handle_Scroll(HID_Dial.count);
		GUI.DrawScreen();
	}
};

void GUI_obj::DrawScreen(){
	MenuPage[Current_Page].Draw();
	Display.gotoXY(48, 0);
	Display.Write(to_char(testValue));
};

void GUI_obj::TestScreen(){
		Display.Clear();
		Display.gotoXY(0,0), Display.Write("Mode:TimeLps");
		Display.gotoXY(0,1), Display.Write("RUN");
		Display.gotoXY(0, 2), Display.Write("RETURN");
		Display.gotoXY(0, 3), Display.Write("Program");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Return);
		Display.gotoXY(0, 4), Display.Write("Settings");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
		Display.gotoXY(0, 5), Display.Write("About");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
};

void GUI_obj::setMenuPosition(MenuPageList page){
	//check to make sure the menu exists
	if((page < END_OF_MENU) && (page >= 0)) Current_Page = page;
};

void GUI_obj::Handle_Button(Button::ButtonStates state){
	HID_Button.Changed = false;
	
	MenuPage_obj *page = &MenuPage[Current_Page];
	MenuItem_obj *item = &page->MenuItem[page->CursorPosition];
	
	switch(state){
		case Button::PRESS:
			//item->itemSelect();
			break;	
			
		case Button::RELEASE:
			item->itemSelect();
			break;
			
		default:
			break;
	};
};

void GUI_obj::Handle_Scroll(int8_t scrollChange){
	HID_Dial.Changed = false;
	
	MenuPage[Current_Page].setCursorPosition(HID_Dial.count);
	HID_Dial.count = 0;
};

//___________________String Conversions_______________________

char *to_char(int16_t value){
	static char buff[7];
	return itoa(value, buff, 10);
};

char *to_char(float value){
	static char buff[10];
	return dtostrf(value, 5, 1, buff);	
};
