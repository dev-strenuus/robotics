#include <functional>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <gazebo/physics/Joint.hh>
#include <stdio.h>
#include <time.h>
#include <chrono>
namespace gazebo
{
  class Vel : public ModelPlugin
  {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

 int i=0;
  private: physics::ModelPtr model;

private: physics::JointPtr joint1_;
private: physics::JointPtr joint2_;

 	private: event::ConnectionPtr updateConnection;
  
  
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      this->model = _parent;
	this->joint1_ = this->model->GetJoint("left_joint");
        this->joint2_ = this->model->GetJoint("right_joint"); 
	
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&Vel::OnUpdate, this));
    }
public: void OnUpdate()
    {


std::chrono::time_point<std::chrono::system_clock> foo = std::chrono::system_clock::now();
	   auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(foo - now);
		if(milliseconds.count()<50)
			return;
	    now = std::chrono::system_clock::now();
           double vel_left =  this->joint1_->GetVelocity(0);
double vel_right =  this->joint2_->GetVelocity(0);
//this->model->SetLinearVel(ignition::math::Vector3d(.3, 0, 0));
            printf ("%d: left_vel -> %lf, right_vel -> %lf\n",i++,  vel_left, vel_right);
    }
 
  };

  GZ_REGISTER_MODEL_PLUGIN(Vel)
}
