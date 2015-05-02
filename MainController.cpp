/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>
#include "MainController.h"
#include "AxisController.h"
#include "GUI.h"

MainController_obj *ControllerPointer;
volatile uint16_t OverflowCounter = 0;

//uint8_t MainController_obj::NUM_AXIS = sizeof(Axis) / sizeof(Axis[0]);

MainController_obj::MainController_obj()
{
	ControllerPointer = this; //store location of Axis objects for ISR
  //this->NUM_AXIS = sizeof(Axis)/sizeof(Axis[0]);
};

//void SetModeType(ModeType modeSelect);
//Storage
//void DeleteMove()
//bool SaveMove(char label[LABEL_LENGTH])
//void LoadMove(char label[LABEL_LENGTH])
//Program Controls
void MainController_obj::RunBuffer()
{
  CalculateAllMoves();
  //PrintMoves();

  for (int i = 0; i <= Axis[0].Motion.Segment[0].finish.steps; i++) {
    //Serial.print("Axis1: "), Serial.print(Axis[0].Motion.StepTime(i)), Serial.print(", Axis2: "), Serial.println(Axis[1].Motion.StepTime(i));
    //volatile uint32_t temp = Axis[0].Motion.StepTime(i);
  }
}

void MainController_obj::RunMove()
{
  CalculateAllMoves();
  //PrintMoves();
  LoadBuffers();
  //Buffer_obj *activeBuffer = &Axis[0].Buffer;
  //Axis[0].Buffer.TimerCompare[0] = 5000;
  //Axis[1].Buffer.TimerCompare[0] = 5000;
  
  //temporarily set direction
  PORTB = (1 << PORTB7);
  PORTB = (1 << PORTB6);

  cli(); //disable interrupts
  
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same with B
  TCCR1B |= (1 << CS11); //set prescale to 8

  TIMSK1 = 0;
  //TIMSK1 &= ~(1 << OCIE1A); //disable compareA interrupt
  //TIMSK1 &= ~(1 << OCIE1B);
  //TIFR1 &= ~(1 << OCF1A); //clear compare A interrupt flag
  //TIFR1 &= ~(1 << OCF1B); //clear compare B interrupt flag

  OCR1A = Axis[0].Buffer.TimerCompare[0]; //set the first compare value
  OCR1B = Axis[1].Buffer.TimerCompare[0];

  OverflowCounter = 0xFFFF; //set overflows to overflow
  handle_overflow_interrupt(); //run once to enable first steps
  TCNT1 = 0; //set timer1 to zero
  TIMSK1 |= (1 << TOIE1); //enable overflow interrupt for Timer1
  //TIFR1 |= (1 << TOV1); //set overflow flag by clearing bit

  sei(); //enable interrupts
    
  while (!Axis[0].Buffer.Finished && !Axis[1].Buffer.Finished) {
    Buffer();
    /*
    if(PrintNumber != FillNumber){
      uint32_t totaltime = ((uint32_t)BufferPull[PrintNumber] << 16) + BufferTim[PrintNumber];
      Serial.println(totaltime);
      PrintNumber++;
      if(PrintNumber >= 50) PrintNumber = 0;
    }
    */
  }
  PORTE |= 0b01000000; //twiddle on

  //Serial.println("Printing Pull Positions");
  //for(int i = 0; i < 100; i++){
  //  Serial.println(BufferPull[1][i]);
  //}

  //Check motor home
}

/*
void MainController_obj::RunMoveTEST(){
  CalculateAllMoves();
  //PrintMoves();
  LoadBuffers();
  while (!Axis[0].Buffer.Finished){
    Buffer();	
	for(uint8_t i = 0; i < NUM_AXIS; i++){
		Axis[i].Buffer.PullPos++;
		if(Axis[i].Buffer.PullPos == BUFFER_SIZE) Axis[i].Buffer.PullPos = 0;
	}
  }
}
*/

void MainController_obj::PauseMove() {
  //disable ISR
}
void MainController_obj::HomeMove() {
  //reset buffers
  //move motor home
}
//Motor Controls
//void goToTime(float seconds);
//void goToPosition(int32_t steps);
//Manipulation
//void SetLooping(bool enable);
//bool ScaleMove(float scaleFactor)
//bool ReverseMove();


void MainController_obj::RunVideo() {
}

void MainController_obj::RunTimelapse(){
	//frames_timelapse = 10;
	CalculateAllMoves();
	float moveTime = getMoveTime();
	float frame_delay = moveTime / (frames_timelapse - 1);
	int16_t frame_current = 1;
	while(frame_current <= frames_timelapse){
		
		//shutter down
		frame_current++;
		Display.Clear();
		Display.gotoXY(0,0);
		Display.Write("Pic:"), Display.Write((int16_t)frame_current);
		PORTF |= (1<<PORTF7);
		//shutter off
		
		_delay_ms(3000);
		PORTF &= ~(1<<PORTF7);
		
		float nextframe_time = frame_delay * frame_current;
		if (nextframe_time > moveTime) nextframe_time = moveTime;
		gotoTimeTEMP(nextframe_time);
	}
}

void MainController_obj::RunAnimation(){}

void MainController_obj::CalculateAllMoves() {
  for (int i = 0; i < NUM_AXIS; i++) {
    Axis[i].Motion.CalculateMove();
  }
}

void MainController_obj::LoadBuffers() {
	for (int i = 0; i < NUM_AXIS; i++) {
		if(Axis[i].Buffer.Enabled){
			Axis[i].Buffer.Reset();
			Axis[i].Buffer.Fill();
		}
		else Axis[i].Buffer.Finished = true;
	}
}

void MainController_obj::Buffer() {
  for (int i = 0; i < NUM_AXIS; i++) {
    Axis[i].Buffer.Next();
  }
}

void MainController_obj::PrintMoves() {
  for (int i = 0; i < NUM_AXIS; i++) {
    //Axis[i].Motion.PrintMove();
  }
}

float MainController_obj::getMoveTime(){
	float longestTime = 0;
	for(uint8_t i = 0; i < NUM_AXIS; i++){
		float totalTime_axis = Axis[i].Motion.totalSeconds;
		if(totalTime_axis > longestTime) longestTime = totalTime_axis;
	}
	return longestTime;
}

void MainController_obj::gotoStep(AxisController_obj *target, int32_t step){
	
	//target->Motion.gotoSegment.start.set(target->getCurrentPosition(), 0);
	//target->Motion.gotoSegment.finish.set(step, 2);
	//target->Motion.gotoSegment.CalcSegment();
	
	
}

void MainController_obj::gotoStep(AxisController_obj *target, float time_seconds){
	//gotoStep(target, target->Motion.getStep_AtTime(time_seconds));
	
}

void MainController_obj::gotoTest(){
	CalculateAllMoves();
	for(uint8_t i = 0; i <= 20; i++){
		volatile int32_t temp0 = Axis[0].Motion.getStep_AtTime(i * (10.0/20.0));
	}
}

void MainController_obj::gotoTimeTEMP(float time_seconds, bool spd){
	//CalculateAllMoves();
	//shitty temp program
	int32_t position[NUM_AXIS];
	int32_t dist[NUM_AXIS];
	bool dir[NUM_AXIS];
	uint8_t enabled = 0; //disable all motors
	
	for(uint8_t i = 0; i < NUM_AXIS; i++){
		position[i] = MICROSTEPS * Axis[i].Motion.getStep_AtTime(time_seconds);
		dir[i] = 0;
		//set direction
		IO_pin_struct *pin = &Axis[i].motorPin.directionPin;
		*pin->_port &= ~pin->_bit; //clear pin
		if(position[i] > Axis[i].currentPosition){
			dir[i] = 1;
			*pin->_port |= pin->_bit;//set pin if +direction
		}
		if(position[i] != Axis[i].currentPosition) enabled |= (1<<i); //enable stepping
		//set direction
	}
	
	//move motors
	while(enabled){
		//turn step pin HIGH
		for(uint8_t i = 0; i < NUM_AXIS; i++){
			//don't step if finished
			if((enabled & (1<<i))){
				IO_pin_struct *stepPin = &Axis[i].motorPin.stepPin;
				*stepPin->_port |= stepPin->_bit; //Step HIGH
				if(dir[i]) Axis[i].currentPosition++;
				else Axis[i].currentPosition--;
				if(Axis[i].currentPosition == position[i]) enabled &= ~(1<<i);  //disable stepping
			}
		}
		_delay_us(1);
		//at least 1us for step signal
		for(uint8_t i = 0; i < NUM_AXIS; i++){
			IO_pin_struct *stepPin = &Axis[i].motorPin.stepPin;
			*stepPin->_port &= ~stepPin->_bit;
		}
		//motor step delay
		if(spd) _delay_us(500);
		else _delay_us(2000);
	}
}

void MainController_obj::goToTime(float seconds){
	//calculate position of each axis at given time
	//setup tempSegments for each axis
	//initialize buffer/ISR for each axis
	CalculateAllMoves(); //calculates data for main path
	moveRunning = 0;
	for(uint8_t i = 0; i < NUM_AXIS; i++){
		//AxisController_obj *axis = Axis[i].Motion
		int32_t startPos = Axis[i].currentPosition / MICROSTEPS;
		int32_t finishPos = Axis[i].Motion.getStep_AtTime(seconds);
		int32_t steps = finishPos - startPos;
		
		//method for calculating a time for the move
		//acceleration based
		//float moveTime = 2.0 * sqrt(abs(steps) / 1000.0); //accel based, a = 1000step/sec^2
		float moveTime = 2.0 * abs(steps) / 500.0;
		if(moveTime < 0.5) moveTime = 0.5;
		
		Axis[i].Motion.transitionSegment.start.set(startPos,0);
		Axis[i].Motion.transitionSegment.finish.set(finishPos,moveTime);
		Axis[i].Motion.transitionSegment.smoothing = 10;
		Axis[i].Motion.transitionSegment.CalcSegment();
		Axis[i].Buffer.attatchTransition();
		
		if(steps != 0){
			Axis[i].Running = true;
			Axis[i].Buffer.Enabled = true;
			moveRunning |= (1 << i);
		}
		else{
			Axis[i].Running = false;
			Axis[i].Buffer.Enabled = false;
		}
	}
	CalculateAllMoves(); //calculate again for data in transition segment
	LoadBuffers();
	
	setupISRs();
    
	while (moveRunning != 0) {
		Buffer();
	}
}

void MainController_obj::goToPosition(AxisController_obj *axis, float position){
	//calculate step using position and coef
	//call gotoStep
}

void MainController_obj::setupISRs(){
	PORTB = (1 << PORTB7);
	PORTB = (1 << PORTB6);

	cli(); //disable interrupts
	
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same with B
	TCCR1B |= (1 << CS11); //set prescale to 8

	TIMSK1 = 0;

	OCR1A = Axis[0].Buffer.TimerCompare[0]; //set the first compare value
	OCR1B = Axis[1].Buffer.TimerCompare[0];

	OverflowCounter = 0xFFFF; //set overflows to overflow
	handle_overflow_interrupt(); //run once to enable first steps
	TCNT1 = 0; //set timer1 to zero
	TIMSK1 |= (1 << TOIE1); //enable overflow interrupt for Timer1

	sei(); //enable interrupts
}


//---------------------ISRs------------------------------------

inline void handle_overflow_interrupt() {
	//PORTE |= 0b01000000; //twiddle on
	//increase overflow counter
	//each motor: check to see if the overflow of the next step is equal to current overflow

	OverflowCounter++;
	AxisController_obj *axis;
	axis = &ControllerPointer->Axis[0];
	if( (axis->Running) && (axis->Buffer.OverflowCompare[axis->Buffer.PullPos] == OverflowCounter) ) TIMSK1 |= (1 << OCIE1A); // enable match 1A interrupt
	axis = &ControllerPointer->Axis[1];
	if( (axis->Running) && (axis->Buffer.OverflowCompare[axis->Buffer.PullPos] == OverflowCounter) ) TIMSK1 |= (1 << OCIE1B); // enable match 1B interrupt
	//if(Axis[2].Buffer.OverflowCompare[Axis[2].Buffer.PullPos] == OverflowCounter) TIMSK1 |= (1 << OCIE1C);  // enable match 1C interrupt
	//if(Axis[3].Buffer.OverflowCompare[Axis[3].Buffer.PullPos] == OverflowCounter) TIMSK3 |= (1 << OCIE3A);  // enable match 3A interrupt
	//if(Axis[4].Buffer.OverflowCompare[Axis[4].Buffer.PullPos] == OverflowCounter) TIMSK3 |= (1 << OCIE3B);  // enable match 3B interrupt
	//if(Axis[5].Buffer.OverflowCompare[Axis[5].Buffer.PullPos] == OverflowCounter) TIMSK3 |= (1 << OCIE3C);  // enable match 3C interrupt

	//PORTE &= 0b10111111;  //twiddle off
}

inline void handle_timer_interrupt(uint8_t targetAxis, volatile uint8_t *TIMSKn, volatile uint16_t *OCRnx, uint8_t OCIEnx)
{
	Buffer_obj *activeBuffer = &ControllerPointer->Axis[targetAxis].Buffer; //buffer pointer
	stepper_DriverPins *activeMotor = &ControllerPointer->Axis[targetAxis].motorPin; //motor port pointer
	
	//set direction pin, and increment current position
	if(activeBuffer->Direction[activeBuffer->PullPos]){
		*activeMotor->directionPin._port |= activeMotor->directionPin._bit;
		ControllerPointer->Axis[targetAxis].currentPosition++;
	}
	else{
		*activeMotor->directionPin._port &= ~activeMotor->directionPin._bit;
		ControllerPointer->Axis[targetAxis].currentPosition--;
	}
	
	//step pin high, hold for at least 1us
	*activeMotor->stepPin._port |= activeMotor->stepPin._bit; //turn step port on. must stay on for about 1us
  
	//Increment Buffer position
	activeBuffer->PullPos = (activeBuffer->PullPos + 1) & (BUFFER_SIZE - 1); //ring buffer, by removing unused bits
	//if (activeBuffer->PullPos == BUFFER_SIZE) activeBuffer->PullPos = 0;
	
	/*
	Check if the buffer has eaten it tail, meaning the ISR is trying to pull from
	where the buffer is trying to fill.This signifies the end of the move, 
	since the buffer stopped filling, OR the move was too fast for the CPU
	*/
	if(activeBuffer->PullPos == activeBuffer->FillPos){
		//disable this axis entirely
		ControllerPointer->Axis[targetAxis].Running = false;
		ControllerPointer->moveRunning &= ~(1 << targetAxis); //clear running flag
		*TIMSKn &= ~(1 << OCIEnx); //Disable compare ISR
		
		//check if every axis is no longer running
		if(!ControllerPointer->moveRunning){
			//disable overflow interrupt
			TIMSK1 &= ~(1 << TOIE1); //disable overflow interrupt for Timer1
		}
	}
	
	if (activeBuffer->OverflowCompare[activeBuffer->PullPos] != OverflowCounter) { //if next step isn't in this timer cycle
		*TIMSKn &= ~(1 << OCIEnx); //Disable compare ISR for the remainder of this timer cycle
	}
  
	*OCRnx = activeBuffer->TimerCompare[activeBuffer->PullPos]; //load next timer value, doesn't matter if it's in a future cycle
  
	//ControllerPointer->Axis[targetAxis].currentPosition

	*activeMotor->stepPin._port &= ~activeMotor->stepPin._bit; //turn step pin off
}

//Overflow
ISR(TIMER1_OVF_vect) {
  handle_overflow_interrupt();
}

//Axis
ISR(TIMER1_COMPA_vect) {
  handle_timer_interrupt(0, &TIMSK1, &OCR1A, OCIE1A); //axis, and timer registers
}

ISR(TIMER1_COMPB_vect) {
  handle_timer_interrupt(1, &TIMSK1, &OCR1B, OCIE1B);
}


/*
ISR(TIMER1_COMPC_vect){
  MainController_obj::handle_timer_interrupt(2, &TIMSK1, &OCR1C, OCIE1C);
}

ISR(TIMER3_COMPA_vect){
  MainController_obj::handle_timer_interrupt(3, &TIMSK3, &OCR3A, OCIE3A);
}

ISR(TIMER3_COMPB_vect){
  MainController_obj::handle_timer_interrupt(4, &TIMSK3, &OCR3B, OCIE3B);
}

ISR(TIMER3_COMPC_vect){
  MainController_obj::handle_timer_interrupt(5, &TIMSK3, &OCR3C, OCIE3C);
}
*/

