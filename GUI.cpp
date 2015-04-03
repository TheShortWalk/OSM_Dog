/*
 * GUI.cpp
 *
 * Created: 3/13/2015 12:21:51 AM
 *  Author: Pooter
 */ 

#define F_CPU 16000000
//#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#include <avr/io.h>
#include <util/delay.h>
#include "GUI.h"
#include "LCD_8544.h"
#include "HID_Components.h"

LCD_8544 Display;

//______________________ GENERATE MENU  ______________________________

	MenuItem_obj Items_MainMenu[] = {
		MenuItem_obj("Mode:"),
		MenuItem_obj("RUN"),
		MenuItem_obj("RETURN"),
		MenuItem_obj("Program"),
		MenuItem_obj("Settings"),
		MenuItem_obj("About"),
		MenuItem_obj("Extra1"),
		MenuItem_obj("Extra2"),
		MenuItem_obj("Extra3"),
		MenuItem_obj("Extra4"),
		MenuItem_obj("Extra5"),
		MenuItem_obj("Extra6")
	};
	
	MenuItem_obj Items_SettingsMenu[] = {
		MenuItem_obj("Op1"),
		MenuItem_obj("Op2"),
		MenuItem_obj("Op3")
	};
	
	MenuPage_obj MenuPages[] = {
		MenuPage_obj(Items_MainMenu, sizeof(Items_MainMenu) / sizeof(Items_MainMenu[0])),
		MenuPage_obj(Items_SettingsMenu, sizeof(Items_SettingsMenu)/sizeof(Items_SettingsMenu[0]))
	};
	
	GUI_obj GUI(MenuPages, 2);

//______________________ MENU ITEMS __________________________________
MenuItem_obj::MenuItem_obj(){};

MenuItem_obj::MenuItem_obj(char *button_label){
	this->Label = button_label;
};

MenuItem_obj::MenuItem_obj(char *button_label, bool *button_function){
	this->Label = button_label;
};

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
	};
	
	
void GUI_obj::Begin(){
	Display.Begin();
	Display.Clear();
	this->DrawScreen();
	HID_Dial.Begin();
	HID_Button.Begin();
};

void GUI_obj::Update(){
	if(HID_Change){
		HID_Change = false;
		MenuPage[Current_Page].setCursorPosition(HID_Dial.count);
		HID_Dial.count = 0;
		GUI.DrawScreen();
	}
};

void GUI_obj::DrawScreen(){
	Current_Page = 0;
	MenuPage[Current_Page].Draw();
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

void GUI_obj::setMenuPosition(int8_t setValue){
	//check to make sure the menu exists
	if(setValue < numberOfMenus) Current_Page = setValue;
};

void GUI_obj::Handle_ButtonPress(Button::ButtonStates state){
	MenuPage_obj *page = &MenuPage[Current_Page];
	MenuItem_obj *item = &page->MenuItem[page->CursorPosition]
	switch(item->type){
		case MenuItem_obj::FUNCTION:
			//run functions
			break;
		case MenuItem_obj::FIELD:
			//change scroll target
			break;
		case MenuItem_obj::MENUCHANGE:
			
			break;
		default:
			break;
	}
	//switch
	//case FUNCTION
	//case FIELD
	//case MENUCHANGE
};

void GUI_obj::Handle_Scroll(int8_t scrollChange){
	
};



/*
void setMode(HID_Event event){
	switch(event){
		case BUTTON_PRESS: //exit menu select
			break;
		case SCROLL_UP: //switch mode
			
			break;
		case SCROLL_DOWN: //switch mode
			break;
		default:
			break;
	}
}

void setRun(HID_Event event){
	switch(event){
		case BUTTON_PRESS:
			//run moco program
			break;
		default:
			break;
	}
}
*/
//void setInt