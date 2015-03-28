/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include "S_Curve.h"

Point_obj::Point_obj() {
  this->steps = 0;
  this->seconds = 0;
}

void Point_obj::offset(int offsetSteps, float offsetSeconds) {
  steps += offsetSteps;
  seconds += offsetSeconds;
}

void Point_obj::set(long setSteps, float setSeconds) {
  steps = setSteps;
  seconds = setSeconds;
}

void Point_obj::PrintPoint() {
  //Serial.print("  step: "), Serial.println(steps);
  //Serial.print("  time: "), Serial.println(seconds);
}

//************************************************************
void Segment_obj::offset(int offsetSteps, float offsetSeconds) {
  start.offset(offsetSteps, offsetSeconds);
  finish.offset(offsetSteps, offsetSeconds);
}

void Segment_obj::CalcSegment(){
  PreCalc();
}

//Calculate the TIME of Step in counter ticks
uint32_t Segment_obj::getStepTimer(uint32_t Step)
{
  
  uint32_t ticks;

  switch (GetSubSegment(Step)) {
  case 1:
    //cli();
    //PORTD |= (1<<7); //twiddle on
    ticks = sqrt(Step) * accelFactor_1;
    //ticks = FastMath(Step);//t = sqrt(2*(p-p0)/a);
    //PORTD &= ~(1<<7);  //twiddle off
    break;
  case 2:
    ticks = (Step - midPoint_A.steps) * velFactor + midPoint_A.seconds * 2000000.0;// t = ((p-p0)/v0)+t_A
    break;
  case 3:
    ticks = (finish.seconds - start.seconds)*2000000 - sqrt(midPoint_A.steps * 2 + midPoint_B.steps - Step) * accelFactor_1; //Path_total_time - sqrt(2*(2*Steps_A + Steps_B - Steps)/a)
    //ticks = (midPoint_A.seconds * 2 + midPoint_B.seconds)*2000000 - FastMath(midPoint_A.steps * 2 + midPoint_B.steps - Step); //Path_total_time - sqrt(2*(2*Steps_A + Steps_B - Steps)/a)
    break;
  }
  return ticks + (uint32_t)(start.seconds * 2000000.0);
}

void Segment_obj::PrintSegment() {

}

//Calculates all of the S-curve data 
void Segment_obj::PreCalc()
{
  midPoint_A.steps = abs(finish.steps - start.steps) * smoothing / 20; //Steps_A = total_steps * smoothing / 20
  midPoint_B.steps = abs(finish.steps - start.steps) - 2 * midPoint_A.steps; //Steps_B = total_steps - 2 * Steps_A

  acceleration = (float)square(4 * midPoint_A.steps + midPoint_B.steps) / (float)(2 * midPoint_A.steps * square(finish.seconds - start.seconds)); //a = (4*Steps_A + Steps_B)^2/(2*Steps_A * (Total_time)^2)

  midPoint_A.seconds = sqrt(2 * midPoint_A.steps / acceleration);//t_A = sqrt(2*(p-p0)/a)------ t_A = time at t_A step
  midPoint_B.seconds = (finish.seconds - start.seconds) - 2.0 * midPoint_A.seconds; //t_B = Total_time - 2*t_A

  velocity = (acceleration * midPoint_A.seconds); //v = a * t_A

  accelFactor_1 = sqrt(2.0/acceleration)*2000000;//(sqrt(1.0 / acceleration) * 15625.0);
  accelFactor_2 = (sqrt(2.0 / acceleration) * 15625.0);
  velFactor = 2000000.0 / velocity;

  deltaSteps = abs(finish.steps - start.steps);

  //direction of step
  if (finish.steps - start.steps > 0) Direction = 1;
  else Direction = 0;
}

int Segment_obj::GetSubSegment(long nextStep)
{
  if (nextStep <= midPoint_A.steps) return 1; //if less than or equal to Steps_A : (first velocity line)
  else if (nextStep > midPoint_A.steps + midPoint_B.steps) return 3; //if greater than Steps_A + Steps_B : (third velocity line)
  else return 2; //if between Steps_A and (Steps_A + Steps_B) : (middle velocity line)
}

