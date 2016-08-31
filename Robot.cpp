#include "SPI.h"
#include "WPILib.h"

#define ROTATION_CONSTANT 180


#define PIXY_START_WORD 0xaa55
#define pixy_address 0x6 //Pixy I2C address
#define pixyinfoamount 14 //Pixy Bytes to receive
#define arduino_address 0x8 //Arduino I2C address
typedef unsigned char byte; //Create a Variable Type for 8 bits

class Robot: public IterativeRobot
{
	//Pixy pixy;
	RobotDrive myRobot; // robot drive system
	Joystick stick,xbox; // only joystick
	LiveWindow *lw;
	VictorSP testmotor,testservo;
	//Servo testservo;
	PWM thing;


	char *chara,holder;
 	int hold,toggle,Val,color,oldcolor,pixytestamount,bits,BufferSize;
	float speed,currPos,heldPos,kRot,kVal;
	bool work,latch,turning,latchB;
	int sync = 0;
	int binVal = 0;
	int checksum = 0;
	double timestamp = 0;
	int signature = 0;
	int xCenter = 0;
	int yCenter = 0;
	int width = 0;
	int height = 0;
	int angle = 0;
	int area = 0;
	int lsb = 0;
	int msb = 0;
	int value = 0;

	I2C *pixyi2c; //Creates the Pixy I2C stuff with a pointer
	I2C *arduinoi2c; //Creates the Arduino I2C stuff with a pointer





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
		thing(1),

		speed(0),
		chara(0),
		holder(0),
		hold(0),
		kVal(0),
		Val(0),
		currPos(0),
		heldPos(0),
		BufferSize(14),
		kRot(0),
		work(0),
		toggle(0),
		latch(0),
		turning(0),
		color(2),
		oldcolor(0),
		pixytestamount(0),
		bits(0),
		latchB(0),
		sync(0),
		checksum(0),
		timestamp(0),
		signature(0),
		xCenter(0),
		yCenter(0),
		width(0),
		height(0),
		angle(0),
		area(0)

	{
		pixyi2c = new I2C(I2C::Port::kOnboard, pixy_address); //Initializes the I2C Class for the Pixy
		arduinoi2c = new I2C(I2C::Port::kOnboard, arduino_address); //Initializes the I2C Class for the Arduino
		myRobot.SetExpiration(0.1); //Code from Example
		// gyro.InitGyro();
	}

	//Call this to update pixy data
	/*
	void getPixydata()
	{

		//Setup Variables for the Target
		short targetnumb, x_axis, y_axis, height, width;
		byte targetdata[Val];

		//read from the i2c bus
		pixyi2c->Read(pixy_address, Val, targetdata);

		//Adds Bytes together to make real numbers
		targetnumb = targetdata[4]*10 + targetdata[5];
		x_axis = targetdata[6]*10 + targetdata[7];
		y_axis = targetdata[8]*10 + targetdata[9];
		height = targetdata[10]*10 + targetdata[11];
		width = targetdata[12]*10 + targetdata[13];



		//Show Data on Smart Dashboard
		SmartDashboard::PutNumber("Data",targetdata[bits]);
		SmartDashboard::PutNumber("Target Number", targetnumb);
		SmartDashboard::PutNumber("X Axis", x_axis);
		SmartDashboard::PutNumber("Y Axis", y_axis);
		SmartDashboard::PutNumber("Height", height);
		SmartDashboard::PutNumber("Width", width);

		//To no bog down the I2C Port
		Wait(0.050);
	}
*/
	//Call this to update the Arduino Color
	void LEDcolor(){
		//Checks if Color Is Changed
		color=round(SmartDashboard::GetNumber("LED Color", color)); //Gets Value from User
		if(color!=oldcolor)
		{
			//Send Color to Arduino
			arduinoi2c->Write(arduino_address, color);

			//Set oldcolor to color so Does not run again until new color is entered
			oldcolor=color;

			//To no bog down the I2C Port
			Wait(0.050);
		}
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
			toggle=toggle+1;
			if(toggle==5)
			{
				toggle=1;
			}
		}
		else if(!stick.GetRawButton(6)&&latch)
		{
			latch=false;
		}
		if(toggle==1)
		{
			kVal=10;
		}
		else if(toggle==2)
		{
			kVal=1;
		}
		else if(toggle==3)
		{
			kVal=0.1;
		}
		else if(toggle==4)
		{
			kVal=0.01;
		}

		if(stick.GetRawButton(5)&&!work)
		{
			Val=Val+kVal;
			work=true;
		}
		else if(stick.GetRawButton(3)&&!work)
		{
			Val=Val-kVal;
			work=true;
		}
		else if(!stick.GetRawButton(5)&&!stick.GetRawButton(3)&&work)
		{
			work=false;
		}
		if(stick.GetRawButton(4)&&!latchB)
		{
			latchB=true;
			bits=bits+1;
			if(bits>Val)
			{
				bits=0;
			}
		}
		else if(!stick.GetRawButton(4)&&latchB)
		{
			latchB=false;
		}

		Val=170;
		binVal=0;
		if(Val>=128)
		{
			Val=Val-128;
			binVal = binVal+1;
		}
		binVal=binVal*10;
		if(Val>=64)
		{
			Val=Val-64;
			binVal=binVal+1;

		}
		binVal=binVal*10;
		if(Val>=32)
		{
			Val=Val-32;
			binVal=binVal+1;
		}
		binVal=binVal*10;
		if(Val>=16)
		{
			Val=Val-16;
			binVal=binVal+1;
		}
		binVal=binVal*10;
		if(Val>=8)
		{
			Val=Val-8;
			binVal=binVal+1;
		}
		binVal=binVal*10;
		if(Val>=4)
		{
			Val=Val-4;
			binVal=binVal+1;
		}
		binVal=binVal*10;
		if(Val>=2)
		{
			Val=Val-2;
			binVal=binVal+1;
		}
		binVal=binVal*10;
		if(Val>=1)
		{
			Val=Val-1;
			binVal=binVal+1;
		}




		SmartDashboard::PutNumber("binVal",binVal);
		SmartDashboard::PutNumber("Bits",bits);
		SmartDashboard::PutNumber("kVal",kVal);
		SmartDashboard::PutNumber("Val",Val);
		SmartDashboard::PutNumber("Angle",(heldPos-currPos));
		SmartDashboard::PutNumber("Speed",speed);
		SmartDashboard::PutString("Running...:","Line The Thing");
		//Gets Pixy Values
		byte readBuffer [90];
		pixyi2c->ReadOnly(90, readBuffer);
		//Put to DashBoard







		SmartDashboard::PutNumber("ReadBuffer",readBuffer[Val]);
		SmartDashboard::PutNumber("ReadBuffer 0",readBuffer[0]);
		SmartDashboard::PutNumber("ReadBuffer 1",readBuffer[1]);
		SmartDashboard::PutNumber("ReadBuffer 2",readBuffer[2]);
		SmartDashboard::PutNumber("ReadBuffer 3",readBuffer[3]);
		SmartDashboard::PutNumber("ReadBuffer 4",readBuffer[4]);
		SmartDashboard::PutNumber("ReadBuffer 5",readBuffer[5]);
		SmartDashboard::PutNumber("ReadBuffer 6",readBuffer[6]);
		SmartDashboard::PutNumber("ReadBuffer 7",readBuffer[7]);
		SmartDashboard::PutNumber("ReadBuffer 8",readBuffer[8]);
		SmartDashboard::PutNumber("ReadBuffer 9",readBuffer[9]);
		SmartDashboard::PutNumber("ReadBuffer 10",readBuffer[10]);
		SmartDashboard::PutNumber("ReadBuffer 11",readBuffer[11]);
		SmartDashboard::PutNumber("ReadBuffer 12",readBuffer[12]);
		SmartDashboard::PutNumber("ReadBuffer 13",readBuffer[13]);
		SmartDashboard::PutNumber("ReadBuffer 14",readBuffer[14]);
		SmartDashboard::PutNumber("ReadBuffer 15",readBuffer[15]);
		SmartDashboard::PutNumber("ReadBuffer 16",readBuffer[16]);

		//Checks for new Color and Sends to Arduino
		LEDcolor();
	}

	//Dunno what to do with this
	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
