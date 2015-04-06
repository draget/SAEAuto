/*
*
* Test code for finding the edges of the road...
* T. Drage 2013
*
* Modified by T. Churack 2015
*/


#include "linear.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../Control/BoeingKalman.cpp"

#define MAX_SLOPE	0.4
#define MIN_R	0.1
#define GROUPSIZE 12
#define MAX_X	10
#define MAX_Y	20
#define MIN_X	0.75
#define ALLOWED_VARIATION	1.5
#define CUTOFF_SLOPE	0.75
#define VARIATION_THRESHOLD	0.25


#define pitchDiffIMU  4.0; //In degrees below horizontal. Correct as of 30/3/2015.
//#define heightOfLidar 1.225; //Metres above , as measured by Calvin Yapp and reported in his thesis

double pitchLayer[] = {2.9, 1.58, 0.65, -0.29}; //Pitch of scan layers (0,1,2,3) relative to the LiDAR's pitch (+ve => angle (more) below horizontal,-ve => angle (more) above horizontal), as reported in Calvin Yapp's thesis

int dips = 0;
int reallocs = 0;
int peaked = 0;
int failed = 0;

struct RPOINT {
	int i;
	double x;
	double r;
	double slope;
};

double TwoPi = 4*acos(0);

double imuTime = 0;
double lidarTime = 0;
double tempTime = 0;



int findrpeak(double AvgRHS, std::vector<RPOINT> rightr) {

	std::vector<RPOINT> Maxs;
	std::vector<RPOINT> Mins;

	// Locates local maximum
	for (std::vector<RPOINT>::iterator it=rightr.begin()+floor((GROUPSIZE-1)/2); it!=rightr.end(); it++) { // Loop through all points

		bool max = true;

		double this_r = (*it).r;

		std::vector<RPOINT>::iterator groupend;

		// Deal with hitting the end.
		if(it + floor((GROUPSIZE-1)/2) > rightr.end()) { groupend = rightr.end(); }
		else { groupend = it + floor((GROUPSIZE-1)/2); }

		for(std::vector<RPOINT>::iterator groupit=it-floor((GROUPSIZE-1)/2); groupit<groupend; groupit++) { // Look to either side to see if point is a max.
			if((*groupit).r > this_r) { max = false; }
		}

		if(max == true) { 
			Maxs.push_back(*it);		
		}

	}


	// Look for (deep) local minima.
	for (std::vector<RPOINT>::iterator it=rightr.begin()+floor((GROUPSIZE-1)/2); it!=rightr.end(); it++) { // Loop through all points

		bool min = true;

		double this_r = (*it).r;

		if(this_r > MIN_R) { continue; }

		std::vector<RPOINT>::iterator groupend;

		// Deal with hitting the end.
		if(it + floor((GROUPSIZE-1)/2) > rightr.end()) { groupend = rightr.end(); }
		else { groupend = it + floor((GROUPSIZE-1)/2); }

		for(std::vector<RPOINT>::iterator groupit=it-floor((GROUPSIZE-1)/2); groupit<groupend; groupit++) { // Look to either side to see if point is a max.
			if((*groupit).r < this_r) { min = false; }
		}

		if(min == true) { 
			Mins.push_back(*it);		
		}

	}

	double Max_r = 0;
	int Peak_i = 0;
	double Peak_x;

	double Min_r = 1;
	int Dip_i = 0;
	double Dip_x;

	// Find the absolute max
	for (std::vector<RPOINT>::iterator it=Maxs.begin(); it!=Maxs.end(); it++) {
		if((*it).r > Max_r*0.9 && (*it).slope < MAX_SLOPE) { Max_r = (*it).r; Peak_i = (*it).i; Peak_x = (*it).x; } // If there are close peaks (90%), prefer the outer one.
	}

	// Find the absolute min within reason
	for (std::vector<RPOINT>::iterator it=Mins.begin(); it!=Mins.end(); it++) {
		if(((*it).r*0.8 < Min_r || (*it).r < 0.05) && fabs((*it).x) < fabs(AvgRHS) + ALLOWED_VARIATION) { // If they are almost zero, prefer the outer one.
			Min_r = (*it).r; Dip_i = (*it).i; Dip_x = (*it).x; 		
		} 
	}


		bool CheckDip = false;

	//std::cout << Peak_x << "," << AvgRHS << "," << Dip_x << "\n";
		if(fabs(Peak_x - AvgRHS) > ALLOWED_VARIATION) { // If we are way off look for the peak near the average!

			double ClosestDelta = 1000;
			int Closest_Peak_i;
			double Closest_Max_r;

			for (std::vector<RPOINT>::iterator it=Maxs.begin(); it!=Maxs.end(); it++) {
				//std::cout << (*it).x << " " << fabs((*it).x - AvgRHS) << "\n";
				if(fabs((*it).x - AvgRHS) < ClosestDelta || (fabs((*it).x - AvgRHS) < ClosestDelta*1.5 && (*it).r > Closest_Max_r*0.9)) { 
					Closest_Peak_i = (*it).i; Closest_Max_r = (*it).r; ClosestDelta = fabs((*it).x - AvgRHS); 
				}
			}

			if(Closest_Max_r > MIN_R && ClosestDelta < fabs(Dip_x - AvgRHS)) { reallocs++; return Closest_Peak_i; }	// Only return if the peak is good enough, else dip might be better.
			else { Peak_x = MIN_X; CheckDip = true; } // If we are going to seek a dip next, check conditions based on a reasonable value.
		}

		// If we are off average, but not by too far its profitable to seek the dip.
		if((fabs(Peak_x - AvgRHS) > VARIATION_THRESHOLD || CheckDip) && fabs(Dip_x) > fabs(Peak_x) && fabs(Dip_x - AvgRHS) < ALLOWED_VARIATION) {
			dips++; 
			return Dip_i;

		}

		if(Max_r > MIN_R) {  peaked++; return Peak_i; }

		else { failed++; return 0; }
	
}

void correctForRoll(double rollSin, double rollCos, std::vector<double> *xvalue, std::vector<double> *yvalue, std::vector<double> *zvalue) {
	//double transmat[3][3] = {{1,0,0}, {0, rollCos, -1 * rollSin}, {0, rollSin, rollCos}};
	int index = 0;
	while (index < (*xvalue).size()) {
		(*yvalue)[index] = rollCos*(*yvalue)[index] - rollSin*(*zvalue)[index];
		(*zvalue)[index] = rollSin*(*yvalue)[index] + rollCos*(*zvalue)[index];
		index++;
	}
}

double* findroad(std::string file, double AvgLHS, double AvgRHS, int layer) {

	std::ifstream infile(file.c_str());

	std::vector<double> xvalues, yvalues, zvalues; //x-axis is perpencicular (left-right) to FOV, y-axis is parallel
	
	double vertang; // pitch of scan point relative to ground, ie a laser pulse is vertang degrees below the horizontal

	xvalues.clear();
	yvalues.clear();
	zvalues.clear();
	
	double *Edges;
	Edges = new double[2];
	
	double pitchAngle = 0;
	double startYaw = 0;
	double rollAngle = 0;//IMU::roll;
	
	

	if (infile.is_open()) {

		double horizangle, planarDist;


		while ( infile.good() ) {


			std::string line;
      			getline (infile,line);

			boost::algorithm::trim(line);

			if(line.empty()) { continue; }
		
			std::vector<std::string> LineParts;
			boost::algorithm::split(LineParts, line,  boost::algorithm::is_any_of(","));
			int num;
			if (LineParts[0] == "0" || LineParts[0] == "1" || LineParts[0] == "2" || LineParts[0] == "3") {
				num = boost::lexical_cast<int>(LineParts[0]);
			} else {
				continue;
			}
		
			if(num == layer) {
				//TODO: implement angle compensation

				vertang = pitchAngle + pitchDiffIMU + pitchLayer[layer];
				horizangle = (boost::lexical_cast<double>(LineParts[2])+2880)*TwoPi/11520; //I believe this is horizontal angle
				double absdist = boost::lexical_cast<double>(LineParts[3])/100.0;
				planarDist = cos(vertang)*absdist;
				

				if(planarDist < sqrt(pow(MAX_Y,2) + pow(MAX_X,2))) { 

					std::cout << cos(horizangle)*planarDist << " " << sin(horizangle)*planarDist << '\n'; //TODO: What is this doing?

					xvalues.push_back(cos(horizangle)*planarDist);
					yvalues.push_back(sin(horizangle)*planarDist); 
					zvalues.push_back(sin(vertang)*absdist);
				}
			}
		}
		correctForRoll(0, 0, &xvalues, &yvalues, &zvalues);
		if (xvalues.size() == 0) {
			return NULL;
		}
  	}
	
	infile.close();

		int FoundStart = 0;			// State variable.
		int InitialSearchDirn = 0;		// Used to tell which direction the loop is currently searching.
		double Max_r = 0.0;			// Stores peak correlation value during fitting.
		double Peak_i = 0.0;			// Stores i of above.
		int LBoundaryPriorToRSearch = 0;	// Stores L bracket after initial search.
	
		std::vector<RPOINT> rightr,leftr;
	
		// Iterators used for bracketing the section being examined.
		std::vector<double>::iterator xfirst;
		std::vector<double>::iterator xlast;
		std::vector<double>::iterator yfirst;
		std::vector<double>::iterator ylast;
	
		std::vector<double>::iterator RHS_i = xvalues.begin();	// Stores RH bracket after finding R.
		std::vector<double>::iterator LHS_i = xvalues.begin(); 	// Stores final LH bracket after finding L.
	
		// Number of points included in intial search and minimum for a fit.
		int GroupSize = GROUPSIZE;
	
		// Index that is iterated over.
		int i = xvalues.size() / 2;
	
		//std::cout << "Initialising all good \n";
		while(i > 0 && i < xvalues.size()) {
	
			if((! FoundStart) && InitialSearchDirn == 0) { // Initial search left
				//std::cout << "Initial search left  \n";
				xfirst = xvalues.begin() + i;
				xlast = xvalues.begin() + i-GroupSize;
				yfirst = yvalues.begin() + i;
				ylast = yvalues.begin() + i-GroupSize;
	
				std::vector<double> groupx(xlast,xfirst);
				std::vector<double> groupy(ylast,yfirst);
	
	
	
				Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());
	
				if(fabs(fit.getSlope()) < MAX_SLOPE) {
	
					// Points were found directly to the left so we need to increase the fit right
					// otherwise we want to increase the fit left
					if(i == (xvalues.size() / 2)) { 
						FoundStart = 1; 
						LBoundaryPriorToRSearch = i; 
						i = i + GroupSize + 1; 
					} 
					else { 
						FoundStart = 2; 
						RHS_i = xvalues.begin() + i; 
						i = i - GroupSize - 1; 
					}
		
				}
				else { i--; InitialSearchDirn = 1; } // Interleave left and right searching
	
			}
			else if((! FoundStart) && InitialSearchDirn == 1) { // Initial search right
				//std::cout << "Initial search right \n";
				int j = xvalues.size() - i; // j and i are symmetric about the centre
	
				xfirst = xvalues.begin() + j + GroupSize;
				xlast = xvalues.begin() + j;
				yfirst = yvalues.begin() + j + GroupSize;
				ylast = yvalues.begin() + j;
	
				std::vector<double> groupx(xlast,xfirst);
				std::vector<double> groupy(ylast,yfirst);
	
	
				Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());
	
				if(fabs(fit.getSlope()) < MAX_SLOPE) {
	
					// If the points lie directly to the right of the centre then we go straight to increasing the fit right
					// Otherwise we do so starting in the right spot.
					if(j == (xvalues.size() / 2)) { FoundStart = 1; }
					else { FoundStart = 1; i = j + GroupSize + 1; }
		
					LBoundaryPriorToRSearch = j; // Save this so that we can go back and increase the fit left later.
	
				}
				else { InitialSearchDirn = 0; } // Go back to looking left
	
			}
			else if(FoundStart == 1) { // Increase fit right
				//std::cout << "Increase fit right, i = " << i << " of " << yvalues.size() << "\n";
				xfirst = xvalues.begin() + i;
				yfirst = yvalues.begin() + i;
				//std::cout << "Here0" << "\n";	
				std::vector<double> groupx(xlast,xfirst);
				//std::cout << "Here0.5" << "\n";	
				std::vector<double> groupy(ylast,yfirst);
				//std::cout << "Here1" << "\n";		
	
				Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());
				double r = pow(fit.getCoefficient(),2);
				//std::cout << i << "," << xvalues[i] << "," << r << "," << fit.getSlope() << "\n";

				RPOINT fitpoint;
				fitpoint.i = i;
				fitpoint.x = xvalues[i];
				fitpoint.r = r;
				fitpoint.slope = fabs(fit.getSlope());
				if(fitpoint.slope < CUTOFF_SLOPE && fabs(fitpoint.x) < MAX_X && fabs(fitpoint.x) > MIN_X && yvalues[i] < MAX_Y) { rightr.push_back(fitpoint); }
				//std::cout << "Here2" << "\n";
	
				i++;
	
				if(i == xvalues.size() || fitpoint.slope > CUTOFF_SLOPE) { // We've reached the RHS edge
					//std::cout << "Reached RHS edge\n";
					if(rightr.size() == 0) { Peak_i = xvalues.size()/2; }
					else if(rightr.size() > floor((GROUPSIZE-1)/2) + 1) { Peak_i = findrpeak(AvgRHS, rightr); }
					else { Peak_i = rightr[rightr.size()-1].i; }
					
					i = LBoundaryPriorToRSearch; // Restore i
					if(Peak_i != 0) { 	// We found something, set the RHS bracket.
						RHS_i = xvalues.begin() + Peak_i; 
					}
					else {			// We didn't...
						RHS_i = xvalues.begin() + i;					
					}
					xfirst = xvalues.begin() + i;
					yfirst = yvalues.begin() + i;
					i = i - GroupSize;
					FoundStart = 2;		// Increase fit left next
					Max_r = 0; Peak_i = 0;
				}
	
				
	
			}
			else if(FoundStart == 2) { // Increase fit left
				//std::cout << "Increase fit left" << "\n";
				xlast = xvalues.begin() + i;
				ylast = yvalues.begin() + i;	
	
				std::vector<double> groupx(xlast,xfirst);
				std::vector<double> groupy(ylast,yfirst);
	
				Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());
	
				double r = pow(fit.getCoefficient(),2);
							//	std::cout << i << "," << xvalues[i] << "," << r << "," << fit.getSlope() << "\n";
				RPOINT fitpoint;
				fitpoint.i = i;
				fitpoint.x = xvalues[i]; 
				fitpoint.r = r;
				fitpoint.slope = fabs(fit.getSlope());
				if(fitpoint.slope < CUTOFF_SLOPE && fabs(fitpoint.x) < MAX_X && fabs(fitpoint.x) > MIN_X && yvalues[i] < MAX_Y) { leftr.push_back(fitpoint); }
	
				i--; // Decrement (until we hit the LHS edge and break the while loop)
				if(fitpoint.slope > CUTOFF_SLOPE) { break; }
	
			}
			
	
		}
	
		if(leftr.size() == 0) { Peak_i = xvalues.size()/2 - 1; }
		else if(leftr.size() > floor((GROUPSIZE-1)/2) + 1) { Peak_i = findrpeak(AvgLHS, leftr); }
		else { Peak_i = leftr[0].i;  }
	
		// Check if we found a better LHS edge, otherwise its the one found in the initial search.
		if(Peak_i != 0) {
			LHS_i = xvalues.begin() + Peak_i; 
		}
		else {
			LHS_i = xvalues.begin() + LBoundaryPriorToRSearch; 
		} 
	
		if(LHS_i >= RHS_i) { LHS_i = RHS_i - 1; }
	
		// Find and fit the entire point from edge to edge.
		std::vector<double> groupx(LHS_i,RHS_i);
		std::vector<double> groupy(yvalues.begin() + (LHS_i - xvalues.begin()),yvalues.begin() + (RHS_i - xvalues.begin()));
	
		Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());
	
		//std::cout << file << "," << *(LHS_i) << "," << *(RHS_i) << "," << fit.getSlope() << "," << fit.getIntercept() << '\n'; 

		Edges[0] = *(LHS_i);
		Edges[1] = *(RHS_i);
		
	
	return Edges;

}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void checkIMU(double lidarTime, std::ifstream *imufile) {
	std::string line;
	if ((*imufile).is_open() && (*imufile).good()) {
		if (tempTime == 0) {
			getline (*imufile, line); //Skip first line
		}
		do {
			imuTime = std::max(tempTime,imuTime); //Set here so that the last value imuTime is set at will be the last set of values PRIOR to the current set of lidar data
	  		getline (*imufile, line);
	  		boost::algorithm::trim(line);
	  		if(line.empty()){break;}
			std::vector<std::string> LineParts;
			boost::algorithm::split(LineParts, line,  boost::algorithm::is_any_of(","));
			tempTime = boost::lexical_cast<double>(LineParts[0]);
		} while(tempTime <= lidarTime);
	}
}


int find(char* dir, std::string imuLog) {
	std::vector<std::string> files;

	struct dirent *dp;
	 DIR *dfd;
	 
	 double *Edges;
	 
	 double allEdges[8] = {0.0};
	
	 if ((dfd = opendir(dir)) == NULL)
	 {
	  fprintf(stderr, "Can't open %s\n", dir);
	  return 0;
	 }
	
	 char filename_qfd[100] ;
	std::ifstream imufile(imuLog.c_str());
	checkIMU(lidarTime, &imufile);
	 while ((dp = readdir(dfd)) != NULL)
	 {
	  struct stat stbuf ;
	  sprintf( filename_qfd , "%s/%s",dir,dp->d_name) ;
	  if( stat(filename_qfd,&stbuf ) == -1 )
	  {
	   printf("Unable to stat file: %s\n",filename_qfd) ;
	   continue ;
	  }
	
	  if ( ( stbuf.st_mode & S_IFMT ) == S_IFDIR )
	  {
	   continue;
	   // Skip directories
	  }
	  else
	  {
		files.push_back(filename_qfd);
	  }
	 }
	
	std::sort(files.begin(), files.end());
	std::cout << "here" << "\n";
	
	float MeasNoise[3] = {1,0,0};
	float ProcNoise[3] = {0.1,0.1,0};
	
	
	
	double LHS[4] = {-2.0,-2.0,-2.0,-2.0};
	double RHS[4] = {2.0,2.0,2.0,2.0};
	
	KalmanPVA KlmL[] = {KalmanPVA(2,K_P,LHS[0],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,LHS[1],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,LHS[2],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,LHS[3],0,0,0.2,ProcNoise,MeasNoise)}; //= (KalmanPVA*) malloc (4*sizeof(KalmanPVA));
	KalmanPVA KlmR[] = {KalmanPVA(2,K_P,RHS[0],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,RHS[1],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,RHS[2],0,0,0.2,ProcNoise,MeasNoise),KalmanPVA(2,K_P,RHS[3],0,0,0.2,ProcNoise,MeasNoise)};//= (KalmanPVA*) malloc (4*sizeof(KalmanPVA));
	
	
	
	for (std::vector<std::string>::iterator it=files.begin(); it != files.end(); ++it) {
	
		if (hasEnding(*it, ".lux")) {
	
			std::cout << *it << "\n";
			int i = 0;
			while (i<4) {
				Edges = findroad(*it,LHS[i],RHS[i],i);
				if(Edges == NULL) {
					i++;
					continue;
				}
	
				if(Edges[0] < MIN_X) { KlmL[i].SetMeasurements(Edges[0], 0,0); }
		
				KlmL[i].Update();
			
				if(Edges[1] > MIN_X) { KlmR[i].SetMeasurements(Edges[1], 0,0); }
			
				KlmR[i].Update();
		
				LHS[i] = KlmL[i].GetPositionEstimate();
		
				RHS[i] = KlmR[i].GetPositionEstimate();
			
				std::cout << Edges[0] << "," << Edges[1] << "," << LHS[i] << "," << RHS[i] << "\n";
			
				std::cout << KlmL[i].GetVelocityEstimate() << "," << KlmR[i].GetVelocityEstimate() << "\n" << "\n";
				allEdges[2*i] = Edges[0];
				allEdges[2*i + 1] = Edges[1];
				i++;
			}
		}
	}
	
		std::cout << "Peaked " << peaked << "\n";
		std::cout << "Reallocated " << reallocs << "\n";
		std::cout << "Dips " << dips << "\n";
		std::cout << "Failed " << failed << "\n";
	
		//Edges = findroad("./test.lux", -2.5769,2.75913);//"./curbs1/1382085929.4942639.lux",-2.5769,2.75913);
		//std::cout << Edges[0] << "," << Edges[1] << "\n";
	
		return 0;
}

void iterate(std::string dir) {
    struct dirent *de=NULL;
    DIR *d=NULL;
    std::cout << dir << "\n";
    std::string scandir = dir;
    dir.append("/imu.log");
    
    const char* path = (scandir.append("/luxscan")).c_str();

    d=opendir(path);
    if(d == NULL) {
        perror("Couldn't open luxscan directory");
    }
    
    // Loop while not NULL
    while(de = readdir(d)) {
        if (hasEnding(de->d_name, ".lux")) {
            printf("%s\n",de->d_name);
            find(de->d_name, dir);
        }
    }
    
    closedir(d);
}

int main(int argc, char *argv[]) {
	std::string dir;
    if (argc > 1) {
        int i=1;
        while (i < argc) {
            dir = boost::lexical_cast<std::string>(argv[i]);
            i++;
            std::string scandir = dir;
    		dir.append("/imu.log");
    		scandir.append("/luxscan");
    		find(&scandir[0], dir);
        }
    } else {
        dir = "../../Control/RunFiles/0/luxscan";
        std::cout << dir << "\n";
        char* tmp = &dir[0];
		find(tmp, "../../Control/RunFiles/0/imu.log");
    }
}

