#include "SPI.h"
#include "WPILib.h"

#define ROTATION_CONSTANT 360

class Robot: public IterativeRobot
{
	//Pixy pixy;
	RobotDrive myRobot; // robot drive system
	Joystick stick,xbox; // only joystick
	LiveWindow *lw;
	VictorSP testmotor,testservo;
	//Servo testservo;


	int hold;
	float speed,currPos,heldPos,kRot;
	bool work,toggle,latch,turning;


public:
	void RobotInit() override {
	}
	Robot() :
		//These must be enabled in the order declared above
		myRobot(4,5),	//Originally (1,0)
		stick(5),		//Needs to be in USB 5
		xbox(0),
		lw(LiveWindow::GetInstance()),
		testmotor(0),
		testservo(1),

		speed(0),
		hold(0),
		currPos(0),
		heldPos(0),
		kRot(0),
		work(0),
		toggle(0),
		latch(0),
		turning(0)

	{
		myRobot.SetExpiration(0.1); //Code from Example
		// gyro.InitGyro();
	}


private:
	void AutonomousInit()	//Initialize Autonomous
	{

	}

	void AutonomousPeriodic()	//Start Autonomous
	{

	}
	void TeleopInit()		//Initiate Teleoperated
	{

	}

	void TeleopPeriodic()	//Teleoperated Begins
	{

		kRot=ROTATION_CONSTANT;
		currPos=-stick.GetRawAxis(3)*360;
		if(!stick.GetRawButton(2))
		{
			if(turning)
			{
				turning=false;
				heldPos=currPos;
				testmotor.SetSpeed(0);
			}
			hold=round(heldPos-currPos);
			if(abs(hold%360)>=5)
			{
				if((hold%360)>180)
				{
					testmotor.SetSpeed(((hold%360)-360)/kRot);
				}
				else if((hold%360)<-180)
				{
					testmotor.SetSpeed(((hold%360)+360)/kRot);
				}
				else
				{
					testmotor.SetSpeed((hold%360)/kRot);
				}
			}
		}


		if(stick.GetRawButton(2))
		{
			speed=stick.GetRawAxis(2);
			turning=true;
			testmotor.SetSpeed(speed);
		}
		else
		{
			speed=0;
		}





		if(stick.GetRawButton(6)&&!latch)
		{
			latch=true;
			toggle=!toggle;
		}
		else if(!stick.GetRawButton(6)&&latch)
		{
			latch=false;
		}
		if(toggle)
		{

		}


		//testmotor.SetSpeed(speed);

		//SmartDashboard::PutNumber("X",pixy.blocks[0].x);
		//SmartDashboard::PutNumber("Y",pixy.blocks[0].y);
		SmartDashboard::PutNumber("Angle",(heldPos-currPos));
		SmartDashboard::PutNumber("Speed",speed);
	}

	//Dunno what to do with this
	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
