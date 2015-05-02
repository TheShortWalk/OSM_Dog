/*
 * OSM_Dog_v2.0.cpp
 *
 * Created: 3/1/2015 12:48:17 PM
 *  Author: Pooter
 */ 


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "GUI.h"
//#include "MainController.h"
//#include "LCD_8544.h"


int main(void)
{
	//MainController_obj Moco;
	
	GUI.Begin();	
		//set Step pins as output
		DDRB |= (1 << 7) | (1 << 6);
		//set Direction pins as output
		DDRD |= (1 << 7);
		DDRC |= (1 << 6);
		
		//
		DDRE |= (1 << 6); //pin 7
		
		DDRF |= (1<<PORTF7);//shutter pin as output
		
		PORTB &= ~(1<<PORTB7);
		PORTB &= ~(1<<PORTB6);	


	Moco.Axis[0].Motion.AddSegment(0);
	Moco.Axis[0].Motion.Segment[0].start.set(0, 0);
	Moco.Axis[0].Motion.Segment[0].finish.set(200, 10);
	Moco.Axis[0].Motion.Segment[0].smoothing = 10;

	Moco.Axis[1].Motion.AddSegment(0);
	Moco.Axis[1].Motion.Segment[0].start.set(0, 0);
	Moco.Axis[1].Motion.Segment[0].finish.set(2000, 10);
	Moco.Axis[1].Motion.Segment[0].smoothing = 1;
	
	_delay_ms(1000);
		
    while(1)
    {
			GUI.Update();
			//Moco.RunMove();
			
			//Moco.frames_timelapse = 50;
			//Moco.RunTimelapse();
		
			//GUI.TestScreen();
			//_delay_ms(500);

		//while(1);
		//_delay_ms(5000);
    }
}