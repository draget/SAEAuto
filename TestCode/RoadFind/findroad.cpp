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
	double Max_r = 0.0;
	double Peak_i = 0;


	std::vector<double>::iterator xfirst;
	std::vector<double>::iterator xlast;
	std::vector<double>::iterator yfirst;
	std::vector<double>::iterator ylast;


	int GroupSize = 8;

	int i = xvalues.size() / 2;
	while(i > 0 && i < xvalues.size()) {

		if(! FoundStart) { 

			xfirst = xvalues.begin() + i;
			xlast = xvalues.begin() + i-GroupSize;
			yfirst = yvalues.begin() + i;
			ylast = yvalues.begin() + i-GroupSize;

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(GroupSize,groupx.data(), groupy.data());

			if(fabs(fit.getSlope()) < 0.5) {

				if(i == xvalues.size() / 2) { FoundStart = 1; }
				else { FoundStart = 2; }
	
				i = i - GroupSize - 1;
			}
			else { i--; }

		}
		else if(FoundStart == 1) {

			xfirst = xvalues.begin() + i;
			yfirst = yvalues.begin() + i;	

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);

			if(r > Max_r) { Max_r = r; Peak_i = i; }

			i--;

		}
		else if(FoundStart == 2) {

			xlast = xvalues.begin() + i;
			ylast = yvalues.begin() + i;	

			std::vector<double> groupx(xlast,xfirst);
			std::vector<double> groupy(ylast,yfirst);

			Maths::Regression::Linear fit(groupx.size(),groupx.data(), groupy.data());

			double r = pow(fit.getCoefficient(),2);

			if(r > Max_r) { Max_r = r; Peak_i = i; }

			i--;

		}
		

	}

	if(Max_r > 0.4) { std::cout << "Edges are: " << *(xlast + Peak_i-1) << " " << *(xfirst-1) << '\n'; }


	return 0;

}


