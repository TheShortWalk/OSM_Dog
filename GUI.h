/*
 * GUI.h
 *
 * Created: 3/13/2015 12:22:01 AM
 *  Author: Pooter
 */ 

#ifndef __GUI_H__
#define __GUI_H__

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
//#include "MainController.h"
#include "LCD_8544.h"
#include "HID_Components.h"

#define SCREENSIZE_X 14 //characters per row
#define SCREENSIZE_Y 6 //rows on screen

enum MenuPageList {MAIN, PROGRAM, SETTINGS, ABOUT, END_OF_MENU};

//enum HID_Event {BUTTON_PRESS, BUTTON_RELEASE, SCROLL_UP, SCROLL_DOWN};

//MainController_obj Moco;
//LCD_8544 Display;

//the types of user input
/*
struct HID_Event{
	bool buttonPress;
	int8_t scrollIncrement;
};*/

//One item in the menu
void linkMenus();

//extern uint16_t testValue;
void set_testValue();

int16_t get_testValue();

char *to_char(int16_t value);
char *to_char(float value);


class MenuItem_obj{
public:
	enum ItemType {TEXT, FUNCTION, FIELD, MENUCHANGE};
		
	MenuItem_obj();
	//Label
	MenuItem_obj(char *button_label);
	//Function
	MenuItem_obj(char *button_label, void (*function)()); 
	//Menu change
	MenuItem_obj(char *button_label, MenuPageList menuPage);
	//Data display and modify
	MenuItem_obj(char *button_label, char (*get)());
	//MenuItem_obj(char *button_label, float (*get)(),	void (*set)(int8_t));
	//MenuItem_obj(const char *button_label, bool *scroll_function);
	
	void Draw();
	
	void itemSelect();
	void itemDeselect();
		
	bool selected;
	
	ItemType type;
	char *Label; //location of button label
	void (*buttonFunction)();
	char (*getData)();
	MenuPageList menuLink;
private:

	//char *to_char(int16_t value);
	//char *to_char(float value);
	
};

//One page of the menu
class MenuPage_obj {
public:
	//char Text[];
	//void *targetFunction(HID_Input userInput);
	MenuPage_obj();
	MenuPage_obj(MenuItem_obj *Items, uint8_t listLength);
	
	int8_t CursorPosition; //user controlled
	uint8_t DrawPosition; //which MenuItem is drawn first
	void Draw();
	void setCursorPosition(int8_t scrollValue);
	
	uint8_t numberOfItems; //number of menu items
	MenuItem_obj *MenuItem; //menu items array
	
private:
	
};

class GUI_obj {
public:
	GUI_obj();
	GUI_obj(MenuPage_obj *Pages, uint8_t listLength);
	void Begin();
	void Update();
	void CheckHID(); //check human input devices 
	void DrawScreen();
	
	void TestScreen();
	
	void setMenuPosition(MenuPageList page);
	
	//enum MenuPage {MAIN, TIMELAPSE, VIDEO, ANIMATION, DRAGONFRAME, SETTINGS, ABOUT};
	//enum MocoMode {TIMELAPSE, VIDEO, ANIMATION, DRAGONFRAME};
		
	uint8_t Current_Mode;
	uint8_t Current_Page;
	
	uint8_t numberOfMenus; //number of menu pages
	MenuPage_obj *MenuPage; //menu page array
private:
	void Handle_Button(Button::ButtonStates state);
	void Handle_Scroll(int8_t scrollChange);
};

extern GUI_obj GUI;



//Main Menu
/*
Mode: Timelapse, Video, Animation, DragonFrame //menu page
RUN			//function call
RESET		//function call
Program		//menu page
Settings	//menu page
About		//menu page
*/

#endif //__GUI_H__