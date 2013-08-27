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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

double TwoPi = 4*acos(0);

int main() {

	std::ifstream infile("./409.lux");

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

				//std::cout << cos(angle)*dist << " " << sin(angle)*dist << '\n';

				xvalues.push_back(cos(angle)*dist);
				yvalues.push_back(sin(angle)*dist);

			}


  		}
	
		infile.close();

	}

	int FoundStart = 0;
	int InitialSearchDirn = 0;
	double Max_r = 0.0;
	double Peak_i = 0.0;
	int LBoundaryPriorToRSearch = 0;

	double Slope = 0;
	double Intercept = 0;

	std::vector<double>::iterator xfirst;
	std::vector<double>::iterator xlast;
	std::vector<double>::iterator yfirst;
	std::vector<double>::iterator ylast;


	int GroupSize = 8;

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

			if(fabs(fit.getSlope()) < 0.5) {

				// Points were found directly to the left so we need to increase the fit right
				// otherwise we want to increase the fit left
				if(i == (xvalues.size() / 2)) { FoundStart = 1; LBoundaryPriorToRSearch = i; } 
				else { FoundStart = 2; i = i - GroupSize - 1; }
	
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

			if(fabs(fit.getSlope()) < 0.5) {

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


			if(r > Max_r) { Max_r = r; Peak_i = i; };

			i++;

			if(i == xvalues.size()) { // We've reached the RHS edge
				i = LBoundaryPriorToRSearch; // Restore i
				if(Max_r > 0.4) { 	// We found something, set the LHS bracket.
					xfirst = xvalues.begin() + Peak_i;
					yfirst = yvalues.begin() + Peak_i;
				}
				else {			// We didn't...
					xfirst = xvalues.begin() + i;
					yfirst = yvalues.begin() + i;
				}
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

			if(r > Max_r) { 
				Max_r = r; 
				Peak_i = i; 			
				Slope = fit.getSlope();
				Intercept = fit.getIntercept(); 
			}

			i--; // Decrement (until we hit the LHS edge and break the while loop)

		}
		

	}

	if(Max_r > 0.4) { std::cout << "Edges are: " << *(xlast + Peak_i-1) << " " << *(xfirst-1) << " Slope: " << Slope << " Intercept: " << Intercept << '\n'; }


	return 0;

}


