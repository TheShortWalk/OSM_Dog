/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.

Controller.h is the overall controller for the system. It contains the top level methods for starting/stopping a motion program.
*/
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef MainController_H
#define MainController_H
#include "AxisController.h"

#define NUM_AXIS 2
#define LABEL_LENGTH 10

//Motor 1
#define DIR_PIN_1 6
#define STEP_PIN_1 11
#define DIR_PORT_1 7
#define STEP_PORT_1 7

//Motor 2
#define  DIR_PIN_2 5
#define  STEP_PIN_2  10
#define DIR_PORT_2 6
#define STEP_PORT_2 6

//Motor 3
#define  DIR_PIN_3  4
#define  STEP_PIN_3  12
#define DIR_PORT_3 0
#define STEP_PORT_3 6

enum ModeType
{
	video,
	timelapse,
	animation,
	dragonframe
};

inline void handle_overflow_interrupt();
inline void handle_timer_interrupt(uint8_t targetAxis, volatile uint8_t *TIMSKn, volatile uint16_t *OCRnx, uint8_t OCIEnx);

class MainController_obj {
public:	
	//static uint8_t NumAxis;

	MainController_obj();
	
	//(stepPort, stepBit, dirPort, dirBit)
	AxisController_obj Axis[NUM_AXIS] = {
		AxisController_obj(&PORTB, 7, &PORTD, 7), //step-11 dir-6
		AxisController_obj(&PORTB, 6, &PORTC, 6) //step-10 dir-5
	 };
	 
	 volatile uint8_t moveRunning;
	
	void SetModeType(ModeType modeSelect);
	bool CheckSpeeds();
	
	//Storage
	void DeleteMove(); //delete all keyframe data
	bool SaveMove(char label[LABEL_LENGTH]); //save keyframes to EEPROM
	void LoadMove(char label[LABEL_LENGTH]); //load keyframes from EEPROM
	
	//Program Controls
	void RunMove(); //Begin the program (moves motors)
	void RunMoveTEST(); //runs a test without moving motors
	void Buffer();
	void RunBuffer();
	void PauseMove(); //Pause the program
	void HomeMove(); //moves motor to start (moves motor)
	
	//Motor Controls
	void gotoTime(float seconds); //drive all motors to time
	void goToPosition(AxisController_obj *axis, float position);
	void jogToTime(float seconds);
	void jogToPosition(AxisController_obj *axis, float position);
	void jogToStep(AxisController_obj *axis, int32_t step);
		
	void gotoStep(AxisController_obj *target, int32_t step);		//move one motor to step position
	void gotoStep(AxisController_obj *target, float time_seconds);	//move one motor to time
	void gotoTimeTEMP(float time_seconds, bool spd = 0);
	
	void gotoTest(); //moves all motors to a time
	
	void eatBuffer();
	

	//Program Manipulation
	void SetLooping(bool enable);
	bool ScaleMove(float positionScale, float timeScale); //scales time
	bool ReverseMove(); //Starts at Finish. Finishes at Start.
	
	void PrintMoves();
	
	//ISR HANDLER's
	//static volatile uint16_t OverflowCounter;
	//inline void handle_overflow_interrupt();
	//inline void handle_timer_interrupt(AxisController_obj *targetAxis, volatile uint8_t *TIMSKn, volatile uint16_t *OCRnx, uint8_t OCIEnx);
	
	//private:
	void CalculateAllMoves();
	void LoadBuffers();
	void RunVideo();
	void RunTimelapse();
	void RunAnimation();
	
	
	float getMoveTime();
	
	uint16_t frames_timelapse; //number of pictures
	
private:
	void setupISRs();

};

#endif