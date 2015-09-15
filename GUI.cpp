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
#include <string.h>
#include "GUI.h"
#include "LCD_8544.h"
#include "HID_Components.h"
//#include "MainController.h"

MainController_obj Moco;


LCD_8544 Display;

int16_t testValue = 0;
float testFloat = 0;

enum mocoModes {TIMELAPSE = 0, VIDEO = 1, ANIMATION = 2, DRAGONFRAME = 3, PC = 4, END_OF_MODES = 5};
int8_t selectedMode = TIMELAPSE;
enum AxisNames {DOLLY, PAN, FOCUS, END_OF_AXIS};
int16_t AxisScale[NUM_AXIS];
int8_t selectedAxis = DOLLY;

void set_testValue(){
	testValue++;
	//if(testValue < 0) testValue = 0;
	//else if(testValue > 50) testValue = 50;
};

void set_Mode(int8_t setVal){
	selectedMode += setVal;
	if(selectedMode >= END_OF_MODES) selectedMode = END_OF_MODES - 1;
	else if(selectedMode < 0) selectedMode = 0;
}

char *get_Mode(){
	switch(selectedMode){
		case TIMELAPSE:
			return "TimeLapse";
			break;
		case VIDEO:
			return "Video";
			break;
		case ANIMATION:
			return "Animate";
			break;
		case DRAGONFRAME:
			return "DragonFrm";
			break;
		case PC:
			return "PC-Link";
			break;
		default:
			selectedMode = TIMELAPSE;
			break;
	}
}

char *get_testValue(){
	//return to_char(testValue);
	return to_char((int16_t)(Moco.Axis[1].currentPosition / MICROSTEPS));
};

char *get_testFloat(){
	return to_char(testFloat);
}

void set_testValue(int8_t setVal){
	testValue += setVal;
	if(testValue > 100) testValue = 100;
}

void set_testFloat(int8_t setVal){
	static int16_t counter = 0;
	counter += setVal;
	testFloat = counter * 0.1;
	if(testFloat > 9999.9) testFloat = 9999.9;
}

void set_Axis(int8_t setVal){
	selectedAxis += setVal;
	if(selectedAxis >= END_OF_AXIS) selectedAxis = END_OF_AXIS - 1;
	else if(selectedAxis < 0) selectedAxis = 0;
}
	
char *get_Axis(){
	switch(selectedAxis){
		case DOLLY:
			return "Dolly";
			break;
		case PAN:
			return "Pan";
			break;
		case FOCUS:
			return "Focus";
			break;
		default:
			selectedMode = DOLLY;
			break;
	}
}

void set_timelapseFrames(int8_t setVal){
	int16_t temp = Moco.frames_timelapse + setVal;
	if(temp < 0) Moco.frames_timelapse = 0;
	else Moco.frames_timelapse = temp;
}

char *get_timelapseFrames(){
	return to_char((int16_t)Moco.frames_timelapse);
}

char *get_Position(){
	return to_char((int16_t)Moco.Axis[selectedAxis].Motion.Segment[0].finish.steps);
}
void set_Position(int8_t setVal){
	Point_obj *target = &Moco.Axis[selectedAxis].Motion.Segment[0].finish;
	target->steps += setVal * AxisScale[selectedAxis];
}

char *get_Time(){
	return to_char(Moco.Axis[selectedAxis].Motion.Segment[0].finish.seconds);
}

void set_Time(int8_t setVal){
	Point_obj *target = &Moco.Axis[selectedAxis].Motion.Segment[0].finish;
	target->seconds += setVal * 0.5;
}

char *get_Smooth(){
	return to_char((int16_t)Moco.Axis[selectedAxis].Motion.Segment[0].smoothing);
}
void set_Smooth(int8_t setVal){
	Segment_obj *target = &Moco.Axis[selectedAxis].Motion.Segment[0];
	int16_t temp = target->smoothing + setVal;
	if(temp < 0) target->smoothing = 0;
	else if(temp > 10) target->smoothing = 10;
	else target->smoothing = temp;
}

//Moco Functions
void runTimelapse(){
	Moco.gotoTime(0.0);
	//Moco.frames_timelapse = 10;
	Moco.RunTimelapse();
	//Moco.gotoTime(testFloat);
}

void runVideo(){
	Moco.gotoTime(0.0);
	Moco.RunMove();
}

void gotoFin(){
	Moco.CalculateAllMoves();
	Moco.gotoTime(Moco.Axis[selectedAxis].Motion.Segment[0].finish.seconds);
}

void gotoStart(){
	Moco.CalculateAllMoves();
	Moco.gotoTime(0);
}



//______________________ GENERATE MENU  ______________________________

MenuItem_obj Items_MainMenu[] = {
	MenuItem_obj(get_Mode, set_Mode),
	MenuItem_obj("Pics:", get_timelapseFrames, set_timelapseFrames),
	MenuItem_obj("Run", runTimelapse),
	MenuItem_obj("Preview", runVideo),
	MenuItem_obj("Reset", gotoStart),
	MenuItem_obj("Path", PROGRAM),
	MenuItem_obj("Settings", SETTINGS),
	MenuItem_obj("About", ABOUT),
	MenuItem_obj("Extra:", get_testValue, set_testValue),
	MenuItem_obj("Haii:", get_testFloat, set_testFloat)
};

MenuItem_obj Items_ProgramMenu[] = {
	MenuItem_obj(get_Axis, set_Axis),
	MenuItem_obj("<  Point:1 >"),
	MenuItem_obj("------------"),
	MenuItem_obj("Pos:", get_Position, set_Position),
	MenuItem_obj("Time:", get_Time, set_Time),
	MenuItem_obj("Smooth:", get_Smooth, set_Smooth),
	MenuItem_obj("------------"),
	MenuItem_obj("start", gotoStart),
	MenuItem_obj("finish", gotoFin),
	MenuItem_obj("Add Point  "),
	MenuItem_obj("Delete Point"),
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

//ItemIncrement
MenuItem_obj::MenuItem_obj(char *button_label, char *(*get)(), void (*set)(int8_t)){
	this->Label = button_label;
	this->getData = get;
	this->setData = set;
	this->type = FIELD;
}

//ListIncrement
MenuItem_obj::MenuItem_obj(char *(*get)(), void (*set)(int8_t)){
	this->getData = get;
	this->setData = set;
	this->type = LIST;
}

void MenuItem_obj::Draw(){
	switch(type){
		case TEXT:
			Display.Write(Label);
			break;
		case FUNCTION:
			Display.Write(Label);
			break;
		case FIELD:
			Display.Write(Label);
			break;
		case LIST:
			Display.gotoAlignX(LEFT), Display.Write("<");
			Display.gotoAlignX(RIGHT), Display.Write(">");
			break;
		case MENUCHANGE:
			Display.Write(Label);
			break;
		
	}	
	
};

void MenuItem_obj::Draw_Data(){
	//Display.Write(Label);
	switch(type){
		case TEXT:
			//write nothing
			break;
		case FUNCTION:
			//Display.Write() //write selected item
			break;
		case FIELD:
			Display.gotoAlignX(RIGHT, strlen(getData()));
			Display.Write(getData()); //write selected number
			break;
		case LIST:
			Display.gotoAlignX(CENTER, strlen(getData()));
			Display.Write(getData());
			break;
		case MENUCHANGE:
			Display.gotoAlignX(RIGHT, 1);
			Display.Write(Symb_Arrow);
			break;
		default:
			break; //write nothing
	}	
}

void MenuItem_obj::itemSelect(){
	switch(type){
		case TEXT:
			break;
		case FUNCTION:
			buttonFunction();
			break;
		case FIELD:
			GUI.itemSelected = true;
			GUI.scrollTarget = setData;
			break;
		case LIST:
			GUI.itemSelected = true;
			GUI.scrollTarget = setData;
			break;
		case MENUCHANGE:
			GUI.setMenuPosition(menuLink); 
			break;
		default:
			break;
	}
};

void MenuItem_obj::itemDeselect(){
		switch(type){
			case TEXT:
				break;
			case FUNCTION:
				buttonFunction();
				break;
			case FIELD:
				GUI.itemSelected = false;
				//GUI.scrollTarget = setData;
				break;
			case LIST:
				GUI.itemSelected = false;
				break;
			case MENUCHANGE:
				GUI.setMenuPosition(menuLink);
				break;
			default:
				break;
		}
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
		if (drawLine == (CursorPosition - DrawPosition)){
			Display.setStyle(INVERT);
			MenuItem[DrawPosition + drawLine].Draw();
			if(!GUI.itemSelected) Display.setStyle(NONE);
			MenuItem[DrawPosition + drawLine].Draw_Data();
			Display.setStyle(NONE);
		}
		else{
			MenuItem[DrawPosition + drawLine].Draw();
			MenuItem[DrawPosition + drawLine].Draw_Data();
		}
		drawLine++;
	}
	//help debug LCD menu scrolling
	//Display.gotoXY(48,0), Display.Write(CursorPosition);
	//Display.gotoXY(48,1), Display.Write(DrawPosition);
};

//handles scrolling of the menu
void MenuPage_obj::setCursorPosition(int8_t scrollValue){
	//Prevent cursor from scrolling off menu
	CursorPosition -= scrollValue;
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
	
	AxisScale[DOLLY] = 100;
	AxisScale[PAN] = 5;
	
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
	//Display.gotoXY(40, 0);
	//Display.Write(itemSelected);
};

void GUI_obj::TestScreen(){
		Display.Clear();
		Display.gotoXY(0,0);
		//char temp[] = "hello";
		Display.gotoAlignX(CENTER, 5);
		Display.Write("hello");
		/*
		Display.gotoXY(0,1), Display.Write("RUN");
		Display.gotoXY(0, 2), Display.Write("RETURN");
		Display.gotoXY(0, 3), Display.Write("Program");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Return);
		Display.gotoXY(0, 4), Display.Write("Settings");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
		Display.gotoXY(0, 5), Display.Write("About");
		Display.gotoAlignX(RIGHT), Display.Write(Symb_Arrow);
		*/
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
			if(itemSelected){
				item->itemDeselect();
			}
			else{
				item->itemSelect();
			}
			break;
			
		default:
			break;
	};
};

void GUI_obj::Handle_Scroll(int8_t scrollChange){
	
	HID_Dial.Changed = false;
	
	if(itemSelected){		
		scrollTarget(HID_Dial.count);
	}
	else{
		MenuPage[Current_Page].setCursorPosition(HID_Dial.count);
	}
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
/*
uint8_t length_str(char *str){
	uint8_t len = 0;
	while(*str){
		len++;
		*str++;
	}
	return len;
}*/
