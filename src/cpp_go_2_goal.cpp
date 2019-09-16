#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"

#include <sstream>

ros::Publisher vel_pub;
ros::Subscriber pose_sub;
turtlesim::Pose turtle_pose;

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message); // Callback function to put turtle pose into a global variable
double getDist(double x1, double y1, double x2, double y2); // Get euclidian distance
void go2Goal(turtlesim::Pose goal_pose, double distance_tolerance); //Proportional controller to make the turtle move to the desired position.


int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "cpp_go_2_goal"); //init node
  ros::NodeHandle n;

  vel_pub = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10); //publish vel_msg
  pose_sub = n.subscribe("/turtle1/pose", 10, poseCallback);// subscribe to pose topic

  ros::Rate loop_rate(0.5);

  turtlesim::Pose pose;
  std::cout << "enter x pose: ";
  std::cin >> pose.x;
  std::cout << "enter y pose: ";
  std::cin >> pose.y;
  pose.theta = 0;

  while(ros::ok()){


      go2Goal(pose, 0.01);

      loop_rate.sleep();

  }


  ros::spin();

  return 0;
}



void poseCallback(const turtlesim::Pose::ConstPtr & pose_message)
{
  turtle_pose.x = pose_message->x;
  turtle_pose.y = pose_message->y;
  turtle_pose.theta = pose_message->theta;
}

double getDist(double x1, double y1, double x2, double y2)
{
  return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)); 
}
void go2Goal(turtlesim::Pose goal_pose, double distance_tolerance)
{
  geometry_msgs::Twist vel_msg;
  ros::Rate loop_rate(100);
  do{
      vel_msg.linear.x = 1.5*getDist(turtle_pose.x, turtle_pose.y, goal_pose.x, goal_pose.y);
      vel_msg.linear.y = 0;
      vel_msg.linear.z = 0; 

      vel_msg.angular.x = 0;
      vel_msg.angular.y = 0;
      vel_msg.angular.z = 6*(atan2(goal_pose.y-turtle_pose.y, goal_pose.x-turtle_pose.x)-turtle_pose.theta);

      vel_pub.publish(vel_msg);
      ros::spinOnce();

      loop_rate.sleep();
  }while(getDist(turtle_pose.x, turtle_pose.y, goal_pose.x, goal_pose.y) > distance_tolerance);

  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;
  vel_pub.publish(vel_msg);
}