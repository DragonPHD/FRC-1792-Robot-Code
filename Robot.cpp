#include "SPI.h"
#include "WPILib.h"

#define ROTATION_CONSTANT 360
#define I2C_SLAVE_ADR 0x6 // ADXL345 I2C device address
#define DEVICE   0x6 // ADXL345 I2C device address
#define ADXL_REG 0x32 // first axis-acceleration-data register on the ADXL345
#define TO_READ     14 // num of bytes we are going to read each time (two bytes for each axis)
#define I2C_CHANNEL 2 // The I2C bus on the upper left-hand side is bus 2
#define ADXL_CONFIG_PORT 0x2D // the configuration port of the ADXL345

typedef unsigned char byte;  // make it more Arduino like

class Robot: public IterativeRobot
{
	//Pixy pixy;
	RobotDrive myRobot; // robot drive system
	Joystick stick,xbox; // only joystick
	LiveWindow *lw;
	VictorSP testmotor,testservo;
	//Servo testservo;
	I2C *i2cChannel2;

	int hold;
	float speed,currPos,heldPos,kRot;
	bool work,toggle,latch,turning;

	const double kUpdatePeriod;

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
		turning(0),
		kUpdatePeriod(0.05)

	{
		myRobot.SetExpiration(0.1); //Code from Example
		// gyro.InitGyro();
		i2cChannel2 = new I2C(I2C::kOnboard, I2C_SLAVE_ADR);
	}


private:
	void AutonomousInit()	//Initialize Autonomous
	{

	}

	void AutonomousPeriodic()	//Start Autonomous
	{

	}
	void initADXL345I2C()
	{
			i2cChannel2->Write(ADXL_CONFIG_PORT, 0);
			i2cChannel2->Write(ADXL_CONFIG_PORT, 16);
			i2cChannel2->Write(ADXL_CONFIG_PORT, 8);
	}

	void getADXL345Data()
	{
		short signature, x_value, y_value,width,height;
		byte buff[TO_READ];

		i2cChannel2->Read(ADXL_REG, TO_READ, buff); //read the acceleration data from the ADXL345

		// each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significant Byte first!!
		// thus we are converting both bytes in to one short int
		signature = (((int)buff[5]) << 8) | buff[4];
		x_value = (((int)buff[7]) << 8) | buff[6];
		y_value = (((int)buff[9]) << 8) | buff[8];
		width = (((int)buff[11] << 8 | buff[10]));
		height = (((int)buff[13] << 8 | buff[12]));

		//print the x, y and z axis info
		// printf("%4d %4d %4d\n", x_axis, y_axis, z_axis);


		SmartDashboard::PutNumber("Buff0",buff[0]);
		SmartDashboard::PutNumber("Buff1",buff[1]);
		SmartDashboard::PutNumber("Buff2",buff[2]);
		SmartDashboard::PutNumber("Buff3",buff[3]);
		SmartDashboard::PutNumber("Buff4",buff[4]);
		SmartDashboard::PutNumber("Buff5",buff[5]);
		SmartDashboard::PutNumber("Buff6",buff[6]);
		SmartDashboard::PutNumber("Buff7",buff[7]);
		SmartDashboard::PutNumber("Buff8",buff[8]);
		SmartDashboard::PutNumber("Buff9",buff[9]);
		SmartDashboard::PutNumber("Buff10",buff[10]);
		SmartDashboard::PutNumber("Buff11",buff[11]);
		SmartDashboard::PutNumber("Buff12",buff[12]);
		SmartDashboard::PutNumber("Buff13",buff[13]);

		//SmartDashboard::PutNumber("Signature", signature);
		//SmartDashboard::PutNumber("X Value", x_value);
		//SmartDashboard::PutNumber("Y Value", y_value);
		//SmartDashboard::PutNumber("Width",width);
		//SmartDashboard::PutNumber("Height",height);

		//It appears that delay is needed in order not to clog the port
		Wait(kUpdatePeriod);

	}
	void TeleopInit()		//Initiate Teleoperated
	{

	}

	void TeleopPeriodic()	//Teleoperated Begins
	{

		initADXL345I2C();
		getADXL345Data();
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
