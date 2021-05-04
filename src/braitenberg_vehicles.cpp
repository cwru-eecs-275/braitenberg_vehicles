#include<ros/ros.h> 
#include<std_msgs/Float64.h> 
#include <stdr_msgs/CO2SensorMeasurementMsg.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <cmath> 

#include "braitenberg_vehicles/cmd_vel_eq.h"

#define ENABLE_ROBOT_STUCK_ACTION 0//adjust this if you'd like to disable the robot's stuck action 

float g_co2_sensor_0_ppm = 0.0; 
float g_co2_sensor_1_ppm = 0.0; 

float g_x_coordinate = 0.0; 
float g_y_coordinate = 0.0; 

geometry_msgs::Twist base_cmd;

void updateCO2Sensor0(const stdr_msgs::CO2SensorMeasurementMsg::ConstPtr& message_holder) 
{ 
  
  //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  g_co2_sensor_0_ppm=message_holder->co2_ppm;
  
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received co2 sensor 0 ppm is %f", g_co2_sensor_0_ppm); 
  
} 

void updateCO2Sensor1(const stdr_msgs::CO2SensorMeasurementMsg::ConstPtr& message_holder) 
{ 
  //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  
  g_co2_sensor_1_ppm=message_holder->co2_ppm;
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor 1 ppm is %f",g_co2_sensor_1_ppm); 
  
} 

void updateLocalOdometryInformation(const nav_msgs::Odometry& odom_info) 
{ 
   //Now let's update our global variable so you can do something with it 
  //In the while loop below. 
  g_x_coordinate = odom_info.pose.pose.position.x;
  g_y_coordinate = odom_info.pose.pose.position.y;
  
} 

int main(int argc, char **argv) 
{ 
  ros::init(argc, argv, "braitenberg_vehicles_py"); //name this node 
  // when this compiled code is run, ROS will recognize it as a node called "braitenberg_vehicles_py" 
  ros::NodeHandle n; // need this to establish communications with our new node 
  
  ros::Rate naptime(1); //create a ros object from the ros “Rate” class; 
  // set our while loop to run at 1 Hz 

  //We'll create a series of subscriber objectes for each of the thermal sensors we expect to have
  ros::Subscriber co2_sensor_0= n.subscribe("co2_sensor_0", 1, updateCO2Sensor0); 
  ros::Subscriber co2_sensor_1= n.subscribe("co2_sensor_1", 1, updateCO2Sensor1);
  ros::Subscriber robot_geometry= n.subscribe("odom", 1, updateLocalOdometryInformation); 
  
  //set up the publisher for the cmd_vel topic
  ros::Publisher cmd_vel_pub_ = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
  
  base_cmd.linear.x = base_cmd.linear.y = base_cmd.angular.z = 0.0; //initialize all vel cmd components
  //to zero 
  
  uint8_t state=0; 
  uint8_t stateCounter=0; 
  
  float vel = 0.0; 
  float ang_vel = 0.0; 

  while (ros::ok())	
  {
	ros::spinOnce(); //let's spin at the top of our loop so we know we've got some good info to act on
	
	cmd_vel_eq(g_co2_sensor_0_ppm, g_co2_sensor_1_ppm, &vel, &ang_vel);

	base_cmd.linear.x = vel;
	base_cmd.angular.z = ang_vel; 

	cmd_vel_pub_.publish(base_cmd); // tell the robot how we want it to move! 
	
	
	//we'll rely on our naptime call to control our CPU consumption 
	naptime.sleep(); 
  }

  //clean up when closing program 

  return 0; 
} 

