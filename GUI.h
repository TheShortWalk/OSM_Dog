/*
 * GUI.h
 *
 * Created: 3/13/2015 12:22:01 AM
 *  Author: Pooter
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
//#include <string.h>
#include "MainController.h"
#include "LCD_8544.h"

#define SCREENSIZE_X 14 //characters per row
#define SCREENSIZE_Y 6 //rows on screen

enum MenuItem {BUTTON, LIST, INCREMENT};
enum HID_Event {BUTTON_PRESS, BUTTON_RELEASE, SCROLL_UP, SCROLL_DOWN};

//MainController_obj Moco;
//LCD_8544 Display;

//the types of user input
/*
struct HID_Event{
	bool buttonPress;
	int8_t scrollIncrement;
};*/

//One item in the menu
class MenuItem_obj{
public:
	MenuItem_obj(char *button_label);
	MenuItem_obj(char *button_label, bool *button_function);
	//MenuItem_obj(const char *button_label, bool *scroll_function);
	
	void Draw();
	
	uint8_t Type;
	const char *Label; //location of button label
	//void *eventTarget(HID_Input);
private:
	
};

//One page of the menu
class MenuPage_obj {
public:
	//char Text[];
	//void *targetFunction(HID_Input userInput);
	MenuPage_obj();
	MenuPage_obj(MenuItem_obj *Items, uint8_t listSize);
	
	uint8_t CursorPosition; //user controlled
	uint8_t DrawPosition; //which MenuItem is drawn first
	void Draw();
	
private:
	uint8_t numberOfItems; //length of menu items array
	MenuItem_obj *MenuItem; //location of menu items
	
	
};

class GUI_obj {
	
public:
	GUI_obj();
	void Begin();
	void CheckHID(); //check human input devices 
	void DrawScreen();
	
	void TestScreen();
	
	//enum MenuPage {MAIN, TIMELAPSE, VIDEO, ANIMATION, DRAGONFRAME, SETTINGS, ABOUT};
	//enum MocoMode {TIMELAPSE, VIDEO, ANIMATION, DRAGONFRAME};
		
	uint8_t Current_Mode;
	uint8_t Current_Page;
	
	
private:

	
};

//Main Menu
/*
Mode: Timelapse, Video, Animation, DragonFrame //menu page
RUN			//function call
RESET		//function call
Program		//menu page
Settings	//menu page
About		//menu page
*/
