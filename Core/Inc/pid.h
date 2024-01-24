/*
 * pid.h
 *
 *  Created on: Jan 22, 2024
 *      Author: S Vedram
 */

#ifndef INC_PID_H_
#define INC_PID_H_


void Compute();
void SetTunings(double Kp, double Ki, double Kd);
void SetSampleTime(int NewSampleTime);
void SetOutputLimits(double Min, double Max);
void SetMode(int Mode);
void Initialize();
void SetControllerDirection(int Direction);



#endif /* INC_PID_H_ */
