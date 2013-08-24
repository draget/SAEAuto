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


	float VAprocnoise[3] = {0.1,0.01,0.001};
	float VAmeasnoise[3] = {2.5,0.05,0.01};

	VAKlmX = new KalmanPVA(3,K_VA,0,0,0,0.2,VAprocnoise,VAmeasnoise);
	VAKlmY = new KalmanPVA(3,K_VA,0,0,0,0.2,VAprocnoise,VAmeasnoise);


	float PVprocnoise[3] = {0.1,0.01,0.001};
	float PVmeasnoise[3] = {5,0.01,0.2};

	PVKlmX = new KalmanPVA(2,K_PV,0,0,0,0.2,PVprocnoise,PVmeasnoise);
	PVKlmY = new KalmanPVA(2,K_PV,0,0,0,0.2,PVprocnoise,PVmeasnoise);


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


void Fusion::GPSTrackAngleUpdate(double GPSTrackAngle) {

	if(CarControl->VectorMagnitude(CurrentVelocity) > 5) { CurrentHeading = GPSTrackAngle; }
	else { CurrentHeading = CarControl->IMU->Yaw + (double)CarControl->CurrentSteeringSetPosn*STEERING_CONSTANT; }

	if(CurrentHeading < 0) { CurrentHeading = 360 + CurrentHeading; }

	if(CarControl->AutoRun) { CarControl->AutoTrackUpdate(CurrentHeading); }

	FusionLog->WriteLogLine("T," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentHeading), true);

}

void Fusion::GPSUpdate(VECTOR_2D GPSPosition, double GPSSpeed) {

	VECTOR_2D GPSVelocity;

	GPSVelocity.x = sin(CarControl->TwoPi*CurrentHeading/360)*GPSSpeed;
	GPSVelocity.y = cos(CarControl->TwoPi*CurrentHeading/360)*GPSSpeed;


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

	if(CarControl->AutoRun) { 
		CarControl->AutoPosUpdate(CurrentPosition); 
		CarControl->AutoSpeedUpdate(CurrentSpeed);
	}
	if(CarControl->RecordActive) { CarControl->MapRecordPosUpdate(CurrentPosition); }
	CarControl->CheckFenceposts(CurrentPosition);

	FusionLog->WriteLogLine("P," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentPosition.x) + "," + boost::lexical_cast<std::string>(CurrentPosition.y), true);
	FusionLog->WriteLogLine("V," + boost::lexical_cast<std::string>(CarControl->TimeStamp()) + "," + boost::lexical_cast<std::string>(CurrentVelocity.x) + "," + boost::lexical_cast<std::string>(CurrentVelocity.y), true);
	

}
