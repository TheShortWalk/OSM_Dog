/*
 * OSM_Dog_v2.0.cpp
 *
 * Created: 3/1/2015 12:48:17 PM
 *  Author: Pooter
 */ 


#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "GUI.h"

MainController_obj Moco;

int main(void)
{
	//GUI_obj GUI;
	
	GUI.Begin();

	
	//GUI.DrawScreen();
	/*
	MainController_obj Moco;
	//LCD_8544 Display;
	//Display.Begin();
	
		DDRB |= (1 << 7) | (1 << 6);
		DDRD |= (1 << 7);
		DDRC |= (1 << 6);
		DDRE |= (1 << 6); //pin 7
		
		PORTB |= (1 << 7);
		PORTB |= (1<< 6);	


	Moco.Axis[0].Motion.AddSegment(0);
	Moco.Axis[0].Motion.Segment[0].start.set(0, 0);
	Moco.Axis[0].Motion.Segment[0].finish.set(5000, 10);
	Moco.Axis[0].Motion.Segment[0].smoothing = 10;

	Moco.Axis[1].Motion.AddSegment(0);
	Moco.Axis[1].Motion.Segment[0].start.set(0, 0);
	Moco.Axis[1].Motion.Segment[0].finish.set(5000, 10);
	Moco.Axis[1].Motion.Segment[0].smoothing = 1;
	*/
	_delay_ms(1000);
		
    while(1)
    {
		//for(uint8_t i = 0; i < 6; i++){
			//GUI.MenuPage[GUI.Current_Page].CursorPosition = i;
			GUI.Update();
			//_delay_ms(500);
		//}
		//GUI.DrawScreen();
		//_delay_ms(500);
		//Moco.RunMove();
		//while(1);
		//_delay_ms(5000);
    }
}