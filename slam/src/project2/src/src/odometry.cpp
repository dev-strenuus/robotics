#include <math.h>
#include <ros/ros.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/Imu.h>
#include <iostream>
#include <string>
#include <thread>
#include <project2/CustomOdometry.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>



using namespace std;

ros::Time currentTime;
ros::Time lastTime;
ros::Publisher customOdomPub;
ros::Publisher testOdomPub;
ros::Subscriber velSub;
ros::Subscriber imuSub;
ros::Publisher odomPub;
tf::TransformBroadcaster *broadcaster;
ros::NodeHandle *n;


double x = 0;
double y = 0;
double theta = 0;
double vLinear = 0;
double vx = 0;
double vy = 0;
double vAngular = 0;
bool ctrl = false;

string source;

void pubPos(){
 geometry_msgs::PoseStamped pos;
 pos.header.frame_id="map";
 pos.header.stamp = currentTime;
 if(pos.header.stamp.nsec >= 800000000)
 {
  pos.header.stamp.nsec = 200000000 - (1000000000 - pos.header.stamp.nsec);
  pos.header.stamp.sec += 1;
 }
 else
  pos.header.stamp.nsec += 200000000;
 pos.pose.position.x = x;
 pos.pose.position.y = y;
 pos.pose.orientation.z = sin(theta/2);
 pos.pose.orientation.w = cos(theta/2);
 testOdomPub.publish(pos);
 project2::CustomOdometry odom;
 odom.pose = pos;
 odom.source = source;
 customOdomPub.publish(odom);

 geometry_msgs::TransformStamped odomTrans;
 odomTrans.header.frame_id = "odom";
 odomTrans.child_frame_id = "base_footprint";

 geometry_msgs::Quaternion odomQuat; 
 odomQuat = tf::createQuaternionMsgFromRollPitchYaw(0,0,theta);
 odomTrans.header.stamp = currentTime; 
 odomTrans.transform.translation.x = x; 
 odomTrans.transform.translation.y = y; 
 odomTrans.transform.translation.z = 0.0;
 odomTrans.transform.rotation = tf::createQuaternionMsgFromYaw(theta);

 nav_msgs::Odometry navOdom;
 navOdom.header.stamp = currentTime;
 navOdom.header.frame_id = "odom";
 navOdom.child_frame_id = "base_footprint";
 
 navOdom.pose.pose.position.x = x;
 navOdom.pose.pose.position.y = y;
 navOdom.pose.pose.position.z = 0.0;
 navOdom.pose.pose.orientation = odomQuat;

 navOdom.twist.twist.linear.x = vx;
 navOdom.twist.twist.linear.y = vy;
 navOdom.twist.twist.linear.z = 0.0;
 navOdom.twist.twist.angular.x = 0.0;
 navOdom.twist.twist.angular.y = 0.0;
 navOdom.twist.twist.angular.z = vAngular;

 
 broadcaster->sendTransform(odomTrans);
 odomPub.publish(navOdom);

}

void velCallback(const geometry_msgs::TwistStamped& msg) {
 vLinear = 0.425*msg.twist.linear.x;
 vAngular = 0.470*msg.twist.angular.z;
 lastTime = currentTime;
 currentTime = ros::Time::now();
 if(!ctrl)
 {
  ctrl = true;
  return;
 }
 double dt = (currentTime - lastTime).toSec();
 vx = vLinear*cos(theta);
 vy = vLinear*sin(theta);
 double dx = vx*dt;
 double dy = vy*dt;
 double dtheta = vAngular*dt;
 x += dx;
 y += dy;
 theta += dtheta;
 pubPos();
}

void imuCallback(const sensor_msgs::Imu& msg){
 if(source!="imu")
  return;
 geometry_msgs::TwistStamped vel;
 vel.twist.linear.x = vLinear/0.425;
 vel.twist.angular.z = msg.angular_velocity.z*(ros::Time::now() - lastTime).toSec();
 velCallback(vel);
}


void getParam(){
 string temp;
 ros::param::get("/odometry/source", temp);
 if(temp != source)
 {
  
  if(temp == "imu")
	source = "imu";
  else
	source = "encoders";
  cout << "The current source is: " << source << endl;
 }
}

void initTimer(){
  
  
  while (true)
  {
    getParam();
    sleep(2);
  }

}


int main(int argc, char **argv) {
 cout << "Starting odometry .. " << endl;
 ros::init(argc, argv, "project2");
 n = new ros::NodeHandle();
 broadcaster = new tf::TransformBroadcaster();
 currentTime = ros::Time::now();
 lastTime = ros::Time::now();
 velSub = n->subscribe("/vel", 1, velCallback);
 imuSub = n->subscribe("/imu", 1, imuCallback);
 ros::param::get("/odometry/source", source);
 cout << "The current source is: " << source << endl;
 customOdomPub = n->advertise<project2::CustomOdometry>("/odometry1", 100);
 testOdomPub = n->advertise<geometry_msgs::PoseStamped>("/testOdometry", 100);
 odomPub = n->advertise<nav_msgs::Odometry>("/odom", 100);
 pubPos();
 thread threadInput (initTimer);
 ros::spin();
 return 0;
}

