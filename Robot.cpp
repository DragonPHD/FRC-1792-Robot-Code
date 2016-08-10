#include "SPI.h"
#include "Pixy.h"
#include "WPILib.h"

class Robot: public IterativeRobot
{
	Pixy pixy;
	RobotDrive myRobot; // robot drive system
	Joystick stick,xbox; // only joystick
	LiveWindow *lw;
	VictorSP testmotor;
	Servo testservo;

	float speed,angle;
	bool work;

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
		angle(0),
		work(0)

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
		//angle=stick.GetRawAxis(0);
		//angle=angle+1;
		//angle=angle*180;
		if(stick.GetRawButton(12)&&!work)
		{
			angle=angle+1;
			work=true;
		}
		else if(stick.GetRawButton(11)&&!work)
		{
			angle=angle-1;
			work=true;
		}
		else if(!stick.GetRawButton(12)&&!stick.GetRawButton(11)&&work)
		{
			work=false;
		}

		pixy.getBlocks(1);




		speed=stick.GetRawAxis(1);
		testmotor.SetSpeed(speed);
		testservo.Set(angle);
		SmartDashboard::PutNumber("X",pixy.blocks[0].x);
		SmartDashboard::PutNumber("Y",pixy.blocks[0].y);
		SmartDashboard::PutNumber("Angle",angle);
		SmartDashboard::PutNumber("Speed",speed);
	}

	//Dunno what to do with this
	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
