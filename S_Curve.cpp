/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

//v = a*t + v0
//p = (1/2)a*t^2 + v0*t + p0

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include "S_Curve.h"

#define PRESCALE 8
#define CLK	16000000
#define TIMER_CONVERSION (CLK / PRESCALE)

Point_obj::Point_obj() {
  this->steps = 0;
  this->seconds = 0;
}

void Point_obj::offset(int16_t offsetSteps, float offsetSeconds) {
  this->steps += offsetSteps;
  this->seconds += offsetSeconds;
}

void Point_obj::set(int32_t setSteps, float setSeconds) {
  this->steps = setSteps;
  this->seconds = setSeconds;
}

void Point_obj::PrintPoint() {
  //Serial.print("  step: "), Serial.println(steps);
  //Serial.print("  time: "), Serial.println(seconds);
}

//************************************************************
void Segment_obj::offset(int16_t offsetSteps, float offsetSeconds) {
  start.offset(offsetSteps, offsetSeconds);
  finish.offset(offsetSteps, offsetSeconds);
}

void Segment_obj::CalcSegment(){
  PreCalc();
}

//Calculate the TIME of Step in counter ticks
//for the love of GOD this function needs to be optimized
//which probably means discarding those floats
//2,000,000 is magic number. It's the clockspeed/precale of the timer. fix it

uint32_t Segment_obj::getStepTimer(uint32_t Step)
{
  
	uint32_t ticks;

	switch (GetSubSegment(Step)) {
		
		case 1: //speed up
		ticks = sqrt(Step) * accelFactor_1;
		//ticks = FastMath(Step);//t = sqrt(2*(p-p0)/a);
		break;
		
		case 2: //constant velocity
		ticks = (Step - midPoint_A.steps) * velFactor + midPoint_A_ticks;// t = ((p-p0)/v0)+t_A
		break;
		
		case 3: //slow down
		ticks = deltaTime_ticks - sqrt(deltaSteps - Step) * accelFactor_1; //Path_total_time - sqrt(2*(2*Steps_A + Steps_B - Steps)/a)
		//ticks = (midPoint_A.seconds * 2 + midPoint_B.seconds)*2000000 - FastMath(midPoint_A.steps * 2 + midPoint_B.steps - Step); //Path_total_time - sqrt(2*(2*Steps_A + Steps_B - Steps)/a)
		break;
		
		case 4: //end of move
		ticks = deltaTime_ticks;
		break;
	
  }
  return ticks + start_ticks;
}

/*
The s-curve is generated using a constant acceleration: a = constant

(acceleration)
|___
|   |
|   |
|___|________________(time)
|             |   |
|             |   |
|             |___|
|
which when integrated creates a trapezoidal velocity curve: v = a*t

(velocity)
|    _________
|   /         \
|  /           \
| /             \
|/_______________\______(time)
which when, once again, integrated creates the s-curve: p = a/2 * t^2 + v0*t + p0

(position)
|               __
|          /
|        / 
|      / 
|    /
|__ __________________(time)
this is a really shitty graph

the final equation is:
	p = a/2 * t^2 + v0*t + p0
	
a	- acceleration	(steps/second^2)
v	- velocity		(steps/second)
p	- position		(steps)
v0	- initial velocity
p0	- initial position

*/
int32_t Segment_obj::getStep(float time_seconds){
	if(deltaSteps == 0) return finish.steps;
	int32_t step;
	float relativeTime;
	volatile uint8_t subsegment = GetSubSegment(time_seconds);
	switch (subsegment){
		
		case 0: //before segment
			step = start.steps;
			break;
			
		case 1: //speed up
			/*
			p = a/2 * t^2 + v0*t + p0
			p0 = 0, v0 = 0
			p = a/2 * t^2
			*/
			step = acceleration / 2.0 * square(time_seconds);
			break;
			
		case 2: //constant velocity
			/*
			p = a/2 * t^2 + v0*t + p0
			a = 0
			p = v0*t + p0
			*/
			step = velocity * (time_seconds - midPoint_A.seconds) + midPoint_A.steps;
			break;
			
		case 3: //speed down
			/*
			solve without using initial velocity:
				p = a/2 * t^2 + v0*t + p0
				v0 = 0
				p = a/2 * t^2 + p0
				p = a/2 * (tA^2 - (tA - (t - tB - tA))^2) + p0
				p = a/2 * (tA^2 - (2*tA + tB - t)^2) + p0
			*/
			//step = (midPoint_A.steps + midPoint_B.steps) + acceleration / 2 * (square(midPoint_A.seconds) - square(midPoint_A.seconds - (time_seconds - midPoint_A.seconds - midPoint_B.seconds)));
			
			/*
			solve using initial velocity:
				p = -a/2 * t^2 + v0*t + p0
			*/						
			relativeTime = time_seconds - midPoint_A.seconds - midPoint_B.seconds;
			step = -acceleration / 2.0 * square(relativeTime) + velocity * relativeTime + (midPoint_A.steps + midPoint_B.steps);
			
			break;
			
		case 4: //after segment
			step = finish.steps;
			break;
			
		default:
			break;
	}
	return step;
}

bool Segment_obj::getDirection(){
	return Direction;
}

void Segment_obj::PrintSegment() {

}

//Calculates all of the S-curve data 
void Segment_obj::PreCalc()
{
	start_ticks = start.seconds * 2000000.0;
	deltaSteps = abs(finish.steps - start.steps);
	deltaTime = finish.seconds - start.seconds;
	deltaTime_ticks = deltaTime * 2000000.0;
	
	midPoint_A.steps = (deltaSteps * smoothing) / 20; //Steps_A = total_steps * smoothing / 20
	midPoint_B.steps = deltaSteps - (2 * midPoint_A.steps); //Steps_B = total_steps - 2 * Steps_A

	acceleration = (float)square(4 * midPoint_A.steps + midPoint_B.steps) / (float)(2 * midPoint_A.steps * square(finish.seconds - start.seconds)); //a = (4*Steps_A + Steps_B)^2/(2*Steps_A * (Total_time)^2)

	midPoint_A.seconds = sqrt(2 * midPoint_A.steps / acceleration);//t_A = sqrt(2*(p-p0)/a)------ t_A = time at t_A step
	midPoint_B.seconds = (finish.seconds - start.seconds) - 2.0 * midPoint_A.seconds; //t_B = Total_time - 2*t_A
	midPoint_A_ticks = midPoint_A.seconds * 2000000.0;

	velocity = (acceleration * midPoint_A.seconds); //v = a * t_A

	accelFactor_1 = sqrt(2.0/acceleration)*2000000;//(sqrt(1.0 / acceleration) * 15625.0);
	accelFactor_2 = (sqrt(2.0 / acceleration) * 15625.0);
	velFactor = 2000000.0 / velocity;

	//direction of step
	if (finish.steps - start.steps > 0) Direction = 1;
	else Direction = 0;
	
}

uint8_t Segment_obj::GetSubSegment(uint32_t nextStep)
{
	if (nextStep <= midPoint_A.steps) return 1; //if less than or equal to Steps_A : (first velocity line)
	else if (nextStep >= deltaSteps) return 4; //beyond the segment
	else if (nextStep > midPoint_A.steps + midPoint_B.steps) return 3; //if greater than Steps_A + Steps_B : (third velocity line)
	else return 2; //if between Steps_A and (Steps_A + Steps_B) : (middle velocity line)
}

//a segment is one s-curve
//speed up - hold speed - speed down
uint8_t Segment_obj::GetSubSegment(float time_seconds){
	//time before segments start
	if(time_seconds <= 0) return 0; 
	//speeding up
	else if(time_seconds <= midPoint_A.seconds) return 1;
	//time after segment
	else if(time_seconds >= deltaTime) return 4;
	//slowing down
	else if(time_seconds > (midPoint_A.seconds + midPoint_B.seconds)) return 3;
	//constant velocity
	else return 2;
}
