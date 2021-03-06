// A node for 'faking' the functionality of a parallel gripper in Gazebo 
// by sending the commanded values from a one joint, which is recieving the
// gripper commands directly, to a 'follower' joint.
//
// Copyright 2020 University of Cincinnati
// All rights reserved. See LICENSE file at:
// https://github.com/MatthewVerbryke/rse_dam
// Additional copyright may be held by others, as reflected in the
// commit history.

#include <string>

#include <ros/ros.h>
#include <std_msgs/Float64.h>

class Listener{
    
    public:
        double command;
    
    void jointCommandCallback(const std_msgs::Float64& msg){
        command = msg.data;
    }
};
    
int main(int argc, char **argv){
    
    // Initialize node
    ros::init(argc, argv, "gripper_mimic");
    ros::NodeHandle nh("~");
    
    // Get ROS Parameters
    std::string side;
    nh.getParam("side", side);
    
    // Set the initial joint command
    double currentGoal = 0.0;
    
    // Loop Rate
    ros::Rate loop_rate(60);
    
    // Create topic names for node
    Listener listener;
    std::string mimicPubName = "/boxbot/" + side + "_gripper_mimic_controller/command";
    std::string mimicSubName = "/boxbot/" + side + "_gripper_controller/command";
    
    // Setup mimic joint command publisher
    ros::Publisher controlPub = nh.advertise<std_msgs::Float64>(mimicPubName, 1);
    
    // Setup joint command subscriber
    ros::Subscriber jointSub = nh.subscribe(mimicSubName, 1, &Listener::jointCommandCallback, &listener);
    
    ros::spinOnce();
    
    // Main Loop
    while (ros::ok())
    {
        // Get the current command
        currentGoal = listener.command;
        
        // Construct and publish the mimic command
        std_msgs::Float64 mimic_msg;
        mimic_msg.data = currentGoal;
        controlPub.publish(mimic_msg);
        
        ros::spinOnce();
        loop_rate.sleep();
    }
}
