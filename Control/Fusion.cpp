/* 
 * File:   Fusion.h
 * Author: T. Drage
 *
 * Sensor Fusion class...
 *
 * Created on 24/8/13
 */


#include <string>
#include <ctime>

#include <boost/lexical_cast.hpp>

#include "BoeingKalman.cpp"

#include "Fusion.h"
#include "Control.h"
#include "Xsens.h"
#include "Logger.h"


/**
 * Purpose: Creates a new instance of the Fusion object.
 * Inputs : None.
 * Outputs: None.
 */
Fusion::Fusion(Control* CarController, Logger* CarLogger) {
 
	CarControl = CarController;
 	Log = CarLogger;	

	// Define the covariance arrays.

	float VAprocnoise[3] = {0.1,0.01,0.001};
	float VAmeasnoise[3] = {2.5,0.05,0.01};

	// Create the first (vel-acc) filter object.

	VAKlmX = new KalmanPVA(3,K_VA,0,0,0,0.2,VAprocnoise,VAmeasnoise);
	VAKlmY = new KalmanPVA(3,K_VA,0,0,0,0.2,VAprocnoise,VAmeasnoise);


	// Same again for pos-vel.

	float PVprocnoise[3] = {0.1,0.01,0.001};
	float PVmeasnoise[3] = {5,0.01,0.2};

	PVKlmX = new KalmanPVA(2,K_PV,0,0,0,0.2,PVprocnoise,PVmeasnoise);
	PVKlmY = new KalmanPVA(2,K_PV,0,0,0,0.2,PVprocnoise,PVmeasnoise);

	CurrentPosition.x = 0;
	CurrentPosition.y = 0;
	CurrentVelocity.x = 0;
	CurrentVelocity.y = 0;
	
	std::string LogPath = CarControl->LogDir + "/fusion.log";

	FusionLog = new Logger(LogPath.c_str());
	
}

/**
 * Purpose: Creates a new instance of the Fusion object.
 * Inputs : An Fusion object.
 * Outputs: None.
 */
Fusion::Fusion(const Fusion& orig) {
}

/**
 * Purpose: Destroys the instance of the Fusion object.
 * Inputs : None.
 * Outputs: None.
 */
Fusion::~Fusion() {
}


/**
 * Purpose: Calculate the cars current heading, function is GPS triggered.
 * Inputs : GPSTrackAngle.
 * Outputs: None.
 */
void Fusion::GPSTrackAngleUpdate(double GPSTrackAngle, int GPSNumSat) {


	// Get the current (scalar) velocity.
	double CurVel = CarControl->VectorMagnitude(CurrentVelocity);
	
	// Compute the cars heading based on IMU yaw and steering position.
	double IMUHeading = CarControl->IMU->Yaw + (double)CarControl->CurrentSteeringSetPosn*STEERING_CONSTANT;
	
	if (GPSNumSat < 5) {
		CurrentHeading = IMUHeading;
	} else {
		
		if(IMUHeading < 0) { IMUHeading = 360 + IMUHeading; }


		// If we have them way off on other sides of north it needs fixing.
		if(GPSTrackAngle - IMUHeading > 180) { 
			if(IMUHeading < (360 - GPSTrackAngle)) { IMUHeading = IMUHeading + 360; } 
			else { GPSTrackAngle = GPSTrackAngle - 360; }
		}
		else if(IMUHeading - GPSTrackAngle > 180) { 
			if(GPSTrackAngle < (360 - IMUHeading)) { GPSTrackAngle = GPSTrackAngle + 360; } 
			else { IMUHeading = IMUHeading - 360; }
		}
		

		// Perform the averaging.
		if(CurVel > 5.0) { CurrentHeading = GPSTrackAngle; }
		else { 
			if(CurVel > 2.0)  { CurrentHeading = (IMUHeading)*(5.0/3.0 - CurVel/3.0) + GPSTrackAngle*(-2.0/3.0 + CurVel/3.0); }
			else { CurrentHeading = IMUHeading; }
		}
	}

	TrackAngleActions();

	boost::thread interpol_Thread = boost::thread(&Fusion::InterpolateTrackAngle, this, IMUHeading);
	interpol_Thread.detach();

}


/**
 * Purpose: Predict forward the car's track angle.
 * Inputs : IMUHeading.
 * Outputs: None.
 */
void Fusion::InterpolateTrackAngle(double IMUHeading) {

	// Calculate the current error.
	double Error = CurrentHeading - IMUHeading;

	boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	// Calculate the new IMU heading.
	double NewIMUHeading = CarControl->IMU->Yaw + (double)CarControl->CurrentSteeringSetPosn*STEERING_CONSTANT;

	// And correct for the error...
	CurrentHeading = NewIMUHeading + Error;

	TrackAngleActions();

}

/**
 * Purpose: Do what needs doing when we have a new track angle.
 * Inputs : None.
 * Outputs: None.
 */
void Fusion::TrackAngleActions() {

	if(CarControl->AutoRun) { CarControl->AutoTrackUpdate(CurrentHeading); }
	
	if(CarControl->ExtLogging) {
		FusionLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentHeading), true);
	}
}

void Fusion::GPSUpdate(VECTOR_2D GPSPosition, double GPSSpeed, int GPSNumSat) {
	
	if (GPSNumSat > 3) {
		
		VECTOR_2D GPSVelocity;

		GPSVelocity.x = sin(CarControl->TwoPi*CurrentHeading/360)*GPSSpeed;
		GPSVelocity.y = cos(CarControl->TwoPi*CurrentHeading/360)*GPSSpeed;

		if(CarControl->ExtLogging) {
			FusionLog->WriteLogLine("UP," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(GPSPosition.x) + "," + boost::lexical_cast<std::string>(GPSPosition.y), true);
			FusionLog->WriteLogLine("UV," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(GPSVelocity.x) + "," + boost::lexical_cast<std::string>(GPSVelocity.y), true);
		}
		
		VECTOR_2D Acceleration = CarControl->IMU->GetAverageAccel(20);
		
		VAKlmX->SetMeasurements(0,GPSVelocity.x, Acceleration.x);
		VAKlmY->SetMeasurements(0,GPSVelocity.y, Acceleration.y);
		VAKlmX->Update();
		VAKlmY->Update();

		CurrentVelocity.x = VAKlmX->GetVelocityEstimate();
		CurrentVelocity.y = VAKlmY->GetVelocityEstimate();
		
		CurrentSpeed = CarControl->VectorMagnitude(CurrentVelocity);

		PVKlmX->SetMeasurements(GPSPosition.x, CurrentVelocity.x,0);
		PVKlmY->SetMeasurements(GPSPosition.y, CurrentVelocity.y,0);

		PVKlmX->Update();
		PVKlmY->Update();
		
		CurrentPosition.x = PVKlmX->GetPositionEstimate();
		CurrentPosition.y = PVKlmY->GetPositionEstimate();
	} else {
		
		VECTOR_2D Acceleration = CarControl->IMU->GetAverageAccel(20);
		
		VAKlmX->SetMeasurements(0,CurrentVelocity.x, Acceleration.x);
		VAKlmY->SetMeasurements(0,CurrentVelocity.y, Acceleration.y);
		VAKlmX->Update();
		VAKlmY->Update();

		CurrentVelocity.x = VAKlmX->GetVelocityEstimate();
		CurrentVelocity.y = VAKlmY->GetVelocityEstimate();
		
		CurrentSpeed = CarControl->VectorMagnitude(CurrentVelocity);

		PVKlmX->SetMeasurements(CurrentPosition.x, CurrentVelocity.x,0);
		PVKlmY->SetMeasurements(CurrentPosition.y, CurrentVelocity.y,0);
		PVKlmX->Update();
		PVKlmY->Update();
		
		CurrentPosition.x = PVKlmX->GetPositionEstimate();
		CurrentPosition.y = PVKlmY->GetPositionEstimate();
		
	}

	PVActions();

	boost::thread interpol_Thread = boost::thread(&Fusion::InterpolatePV, this);
	interpol_Thread.detach();

}


void Fusion::InterpolatePV() {

	VECTOR_2D LastVelocity = CurrentVelocity;
	VECTOR_2D LastPosition = CurrentPosition;

	boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	VECTOR_2D Acceleration = CarControl->IMU->GetAverageAccel(10);

	//Log->WriteLogLine(boost::lexical_cast<std::string>(Acceleration.y));

	CurrentVelocity.x = LastVelocity.x + 0.1*Acceleration.x;
	CurrentVelocity.y = LastVelocity.y + 0.1*Acceleration.y;

	CurrentPosition.x = LastPosition.x + 0.1*CurrentVelocity.x;
	CurrentPosition.y = LastPosition.y + 0.1*CurrentVelocity.y;

	PVActions();

}

void Fusion::PVActions() {

	if(CarControl->AutoRun) { 
		CarControl->AutoPosUpdate(CurrentPosition); 
		CarControl->AutoSpeedUpdate(CurrentSpeed);
	}
	if(CarControl->RecordActive) { CarControl->MapRecordPosUpdate(CurrentPosition); }
	CarControl->CheckFenceposts(CurrentPosition);

	if(CarControl->ExtLogging) {
		FusionLog->WriteLogLine("P," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentPosition.x) + "," + boost::lexical_cast<std::string>(CurrentPosition.y), true);
		FusionLog->WriteLogLine("V," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentVelocity.x) + "," + boost::lexical_cast<std::string>(CurrentVelocity.y), true);
	}

}
