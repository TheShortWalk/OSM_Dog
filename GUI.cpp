/*
 * GUI.cpp
 *
 * Created: 3/13/2015 12:21:51 AM
 *  Author: Pooter
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "GUI.h"
#include "LCD_8544.h"

LCD_8544 Display;

uint8_t CurrentPage = 0;

//********************* MENU ITEMS ****************************

MenuItem_obj::MenuItem_obj(char *button_label){
	this->Label = button_label;
};

MenuItem_obj::MenuItem_obj(char *button_label, bool *button_function){
	this->Label = button_label;
};

void MenuItem_obj::Draw(){
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
};

//********************* MENU PAGES ****************************

MenuPage_obj::MenuPage_obj(){
	
	};
/*	
MenuPage_obj::MenuPage_obj(MenuItem_obj *Items, uint8_t listSize){
	this->MenuItem = Items;
	this->numberOfItems = listSize;
};
*/

void MenuPage_obj::Draw(){
	
};


//********************* GUI ****************************

GUI_obj::GUI_obj(){};
	
void GUI_obj::Begin(){
	Display.Begin();
	Display.Clear();
	TestScreen();
};

void GUI_obj::DrawScreen(){
	
	

};

void GUI_obj::TestScreen(){
		Display.Clear();
		Display.gotoXY(0,0), Display.Write("Mode:TimeLps");
		Display.gotoXY(0,1), Display.Write("RUN");
		Display.gotoXY(0, 2), Display.Write("RETURN");
		Display.gotoXY(0, 3), Display.Write("Program");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
		Display.gotoXY(0, 4), Display.Write("Settings");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
		Display.gotoXY(0, 5), Display.Write("About");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
};

//********************* GENERATE MENU  ****************************


//MenuItem_obj myItem("hello");
//MenuPage_obj myPage(&myItem, 1);

MenuItem_obj item1("1");
MenuItem_obj item2("2");
MenuItem_obj item3("3");

MenuItem_obj item_list[] = {item1, item2, item3};
MenuPage_obj menu_page(item_list, 3);



/*
MenuItem_obj Items_MainMenu[] = {
	MenuItem_obj("Mode:"),
	MenuItem_obj("RUN"),
	MenuItem_obj("RETURN"),
	MenuItem_obj("Program"),
	MenuItem_obj("Settings"),
	MenuItem_obj("About")
};
*/

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