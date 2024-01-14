/*
 * mainpp.cpp
 *
 *  Created on: Jan 13, 2024
 *      Author: S Vedram
 */
#include <main.h>
#include <mainpp.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int16.h>
#include <ros/time.h>

int flag = 0;
int leftenc = 0, leftenco = 0, rightenc = 0, rightenco = 0;
int pos_act_left = 0, pos_act_right = 0;
int rightvel = 0, leftvel = 0, rpm_right = 0, rpm_left = 0;

float demandx = 0;
float demandz = 0;

void cmd_vel_cb( const geometry_msgs::Twist& twist)
{
   demandx = twist.linear.x;
   demandz = twist.angular.z;
 }

ros::NodeHandle nh;
std_msgs::Int16 left_wheel_msg;
ros::Publisher left_wheel_pub("lwheel", &left_wheel_msg);
std_msgs::Int16 right_wheel_msg;
ros::Publisher right_wheel_pub("rwheel", &right_wheel_msg);
ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", cmd_vel_cb );

void publishPos()
{
  left_wheel_msg.data = pos_act_left;
  right_wheel_msg.data = pos_act_right;
  left_wheel_pub.publish(&left_wheel_msg);
  right_wheel_pub.publish(&right_wheel_msg);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	if(GPIO_PIN == LeftWheelEncoderChannelA_Pin)
	{
		if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelA_GPIO_Port, LeftWheelEncoderChannelA_Pin) == 1)
		{
			if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelB_GPIO_Port, LeftWheelEncoderChannelB_Pin) == 1) leftenc++;
			else if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelB_GPIO_Port, LeftWheelEncoderChannelB_Pin) == 0) leftenc--;
		}
		else if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelA_GPIO_Port, LeftWheelEncoderChannelA_Pin) == 0)
		{
			if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelB_GPIO_Port, LeftWheelEncoderChannelB_Pin) == 0) leftenc++;
			else if(HAL_GPIO_ReadPin(LeftWheelEncoderChannelB_GPIO_Port, LeftWheelEncoderChannelB_Pin) == 1) leftenc--;
		}
	}
	else if(GPIO_PIN == RightWheelEncoderChannelA_Pin)
	{
		if(HAL_GPIO_ReadPin(RightWheelEncoderChannelA_GPIO_Port, RightWheelEncoderChannelA_Pin) == 1)
		{
			if(HAL_GPIO_ReadPin(RightWheelEncoderChannelB_GPIO_Port, RightWheelEncoderChannelB_Pin) == 0) rightenc++;
			else if(HAL_GPIO_ReadPin(RightWheelEncoderChannelB_GPIO_Port, RightWheelEncoderChannelB_Pin) == 1) rightenc--;
		}
		else if(HAL_GPIO_ReadPin(RightWheelEncoderChannelA_GPIO_Port, RightWheelEncoderChannelA_Pin) == 0)
		{
			if(HAL_GPIO_ReadPin(RightWheelEncoderChannelB_GPIO_Port, RightWheelEncoderChannelB_Pin) == 1) rightenc++;
			else if(HAL_GPIO_ReadPin(RightWheelEncoderChannelB_GPIO_Port, RightWheelEncoderChannelB_Pin) == 0) rightenc--;
		}
	}
	flag = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	rpm_right = (int)((rightenc - rightenco) * 0.6);
	rpm_left = (int)((leftenc - leftenco) * 0.6);
	rightvel = (int)((rpm_right * 6.28 * 7.5)/(60));
	leftvel = (int)((rpm_left * 6.28 * 7.5)/(60));
	rightenco = rightenc;
	leftenco = leftenc;
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	pos_act_left = leftenc;
	pos_act_right = rightenc;

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->flush();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->reset_rbuf();
}

void setup()
{
	nh.initNode();
	nh.advertise(left_wheel_pub);
	nh.advertise(right_wheel_pub);
	nh.subscribe(sub);
}

void loop()
{
	publishPos();
	nh.spinOnce();
	HAL_Delay(150);
}
