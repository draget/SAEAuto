/*
*
* Test code for finding the edges of the road...
* T. Drage 2013
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

#include "BoeingKalman.cpp"

#define MAX_SLOPE	0.1
#define MIN_R	0.15

double TwoPi = 4*acos(0);


double* findroad(std::string file, double AvgLHS, double AvgRHS) {

	//std::ifstream infile("./curbover.lux");
	//std::ifstream infile("./1380264080.127073.lux");
	//std::ifstream infile("./longrange/test_invert.lux");

	std::ifstream infile(file.c_str());

	std::vector<double> xvalues, yvalues;

	if (infile.is_open()) {

		double angle, dist;

		while ( infile.good() ) {


			std::string line;
      			getline (infile,line);

			boost::algorithm::trim(line);

			if(line.empty()) { continue; }
		
			std::vector<std::string> LineParts;
			boost::algorithm::split(LineParts, line,  boost::algorithm::is_any_of(","));

			if(LineParts[0].compare(0,1,"0") == 0) {
			
				angle = (boost::lexical_cast<double>(LineParts[2])+2880)*TwoPi/11520;
				dist = boost::lexical_cast<double>(LineParts[3])/100.0;

				std::cout << cos(angle)*dist << " " << sin(angle)*dist << '\n';

				xvalues.push_back(cos(angle)*dist);
				yvalues.push_back(sin(angle)*dist);

			}


  		}
	
		infile.close();

	}

	int FoundStart = 0;			// State variable.
	int InitialSearchDirn = 0;		// Used to tell which direction the loop is currently searching.
	double Max_r = 0.0;			// Stores peak correlation value during fitting.
	double Peak_i = 0.0;			// Stores i of above.
	int LBoundaryPriorToRSearch = 0;	// Stores L bracket after initial search.

	// Iterators used for bracketing the section being examined.
	std::vector<double>::iterator xfirst;
	std::vector<double>::iterator xlast;
	std::vector<double>::iterator yfirst;
	std::vector<double>::iterator ylast;

	std::vector<double>::iterator RHS_i = xvalues.begin();	// Stores RH bracket after finding R.
	std::vector<double>::iterator LHS_i = xvalues.begin(); 	// Stores final LH bracket after finding L.

	// Number of points included in intial search and minimum for a fit.
	int GroupSize = 12;

	// Index that is iterated over.
	int i = xvalues.size() / 2;


	while(i > 0 && i < xvalues.size()) {

		if((! FoundStart) && InitialSearchDirn == 0) { // Initial search left

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

			xfirst = xvalues.begin() + i;
			yfirst = yvalues.begin() + i;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);


			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);
							std::cout << i << "," << xvalues[i] << "," << r << " " << fit.getSlope() << "\n";
			// Check if we have a max - must also have a reasonable slope.
			if(r > Max_r && fabs(fit.getSlope()) < MAX_SLOPE) { Max_r = r; Peak_i = i; }

			i++;

			if(i == xvalues.size()) { // We've reached the RHS edge
				i = LBoundaryPriorToRSearch; // Restore i
				if(Max_r > MIN_R) { 	// We found something, set the RHS bracket.
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

			xlast = xvalues.begin() + i;
			ylast = yvalues.begin() + i;	

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);

			if(r > Max_r && fit.getSlope() < MAX_SLOPE) { Max_r = r; Peak_i = i; }

			i--; // Decrement (until we hit the LHS edge and break the while loop)

		}
		

	}

	// Check if we found a better LHS edge, otherwise its the one found in the initial search.
	if(Max_r > MIN_R) {
		LHS_i = xlast + Peak_i-1;
	}
	else {
		LHS_i = xvalues.begin() + LBoundaryPriorToRSearch;
	}

	// Find and fit the entire point from edge to edge.
	std::vector<double> groupx(LHS_i,RHS_i);
	std::vector<double> groupy(yvalues.begin() + (LHS_i - xvalues.begin()),yvalues.begin() + (RHS_i - xvalues.begin()));

	Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

	//std::cout << file << "," << *(LHS_i) << "," << *(RHS_i) << "," << fit.getSlope() << "," << fit.getIntercept() << '\n'; 

	double *Edges = new double[2];

	Edges[0] = *(LHS_i);
	Edges[1] = *(RHS_i);

	return Edges;

}


int main () {

std::vector<std::string> files;

	struct dirent *dp;
 DIR *dfd;

 char *dir ;
 dir = "./roadedges2";


 if ((dfd = opendir(dir)) == NULL)
 {
  fprintf(stderr, "Can't open %s\n", dir);
  return 0;
 }

 char filename_qfd[100] ;


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

/*
float MeasNoise[3] = {0.5,0,0};
float ProcNoise[3] = {0.01,0.01,0};

double *Edges;

double LHS, RHS;

KalmanPVA KlmL = KalmanPVA(2,K_P,0,0,0,0.2,ProcNoise,MeasNoise);
KalmanPVA KlmR = KalmanPVA(2,K_P,0,0,0,0.2,ProcNoise,MeasNoise);

for (std::vector<std::string>::iterator it=files.begin(); it!=files.end(); ++it) {

	Edges = findroad(*it,LHS,RHS);

	KlmL.SetMeasurements(Edges[0], 0,0);

	KlmL.Update();

	KlmR.SetMeasurements(Edges[1], 0,0);

	KlmR.Update();

	LHS = KlmL.GetPositionEstimate();

	RHS = KlmR.GetPositionEstimate();

	std::cout << LS << "," << RHS << "\n";


}
*/

findroad("./roadedges2/1380264053.6823759.lux",0,0);

	return 0;

}

