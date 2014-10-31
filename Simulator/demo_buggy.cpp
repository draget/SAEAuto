/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/*

buggy with suspension.
this also shows you how to use geom groups.

*/


#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "texturepath.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// select correct drawing functions

#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCapsule dsDrawCapsuleD
#endif


// some constants

#define LENGTH 1.8	// chassis length
#define WIDTH 1.4	// chassis width
#define HEIGHT 0.5	// chassis height
#define RADIUS 0.3	// wheel radius
#define STARTZ 1.0	// starting height of chassis
#define CMASS 1		// chassis mass
#define WMASS 0.2	// wheel mass

#define FOLLOW 5


bool OpenState;

bool Run;

boost::thread m_Thread;
boost::thread pos_Thread;
FILE *RXpipe;
FILE *TXpipe;


// dynamics and collision objects (chassis, 3 wheels, environment)

static dWorldID world;
static dSpaceID space;
static dBodyID body[5];
static dJointID joint[4];	// joint[0,1] is the front wheel
static dJointGroupID contactgroup;
static dGeomID ground;
static dSpaceID car_space;
static dGeomID box[1];
static dGeomID sphere[4];
static dGeomID ground_box;
static dGeomID ground_box2;


// things that the user controls

static dReal speed=0,steer=0;	// user commands



// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  int i,n;

  // only collide things with the ground
  int g1 = (o1 == ground || o1 == ground_box || o1 == ground_box2);
  int g2 = (o2 == ground || o2 == ground_box || o1 == ground_box2);
  if (!(g1 ^ g2)) return;

  const int N = 10;
  dContact contact[N];
  n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
  if (n > 0) {
    for (i=0; i<n; i++) {
      contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
	dContactSoftERP | dContactSoftCFM | dContactApprox1;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.slip1 = 0.2;
      contact[i].surface.slip2 = 0.2;
      contact[i].surface.soft_erp = 0.5;
      contact[i].surface.soft_cfm = 0.3;
      dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
      dJointAttach (c,
		    dGeomGetBody(contact[i].geom.g1),
		    dGeomGetBody(contact[i].geom.g2));
    }
  }
}


// start simulation - set viewpoint

static void start()
{
  dAllocateODEDataForThread(dAllocateMaskAll);

  static float xyz[3] = {-5.0f,0.0f,4.0000f};
  static float hpr[3] = {0.0000f,-30.5000f,0.0000f};
  dsSetViewpoint (xyz,hpr);
  printf ("Press:\t'a' to increase speed.\n"
	  "\t'z' to decrease speed.\n"
	  "\t',' to steer left.\n"
	  "\t'.' to steer right.\n"
	  "\t' ' to reset speed and steering.\n"
	  "\t'1' to save the current state to 'state.dif'.\n");
}


// called when a key pressed

static void command (int cmd)
{
  switch (cmd) {
  case 'a': case 'A':
    speed += 0.3;
    fprintf (stderr, "Driving wheel speed set to %g rad/s.\n", speed);
    break;
  case 'z': case 'Z':
    speed -= 0.3;
    fprintf (stderr, "Driving wheel speed set to %g rad/s.\n", speed);
    break;
  case ',':
    steer -= 0.5;
    break;
  case '.':
    steer += 0.5;
    break;
  case ' ':
    speed = 0;
    steer = 0;
    break;
  case '1': {
      FILE *f = fopen ("state.dif","wt");
      if (f) {
        dWorldExportDIF (world,f,"");
        fclose (f);
      }
    }
  }
}


// simulation loop

static void simLoop (int pause)
{
  int i;
  if (!pause) {
    // motor
    dJointSetHinge2Param (joint[2],dParamVel2,-speed);
    dJointSetHinge2Param (joint[2],dParamFMax2,0.1);
    dJointSetHinge2Param (joint[3],dParamVel2,-speed);
    dJointSetHinge2Param (joint[3],dParamFMax2,0.1);

    // steering
    dReal v = steer - dJointGetHinge2Angle1 (joint[0]);
    if (v > 0.1) v = 0.1;
    if (v < -0.1) v = -0.1;
    v *= 10.0;
    dJointSetHinge2Param (joint[0],dParamVel,v);
    dJointSetHinge2Param (joint[0],dParamFMax,0.2);
    dJointSetHinge2Param (joint[0],dParamLoStop,-0.75);
    dJointSetHinge2Param (joint[0],dParamHiStop,0.75);
    dJointSetHinge2Param (joint[0],dParamFudgeFactor,0.1);
    dJointSetHinge2Param (joint[1],dParamVel,v);
    dJointSetHinge2Param (joint[1],dParamFMax,0.2);
    dJointSetHinge2Param (joint[1],dParamLoStop,-0.75);
    dJointSetHinge2Param (joint[1],dParamHiStop,0.75);
    dJointSetHinge2Param (joint[1],dParamFudgeFactor,0.1);

    dSpaceCollide (space,0,&nearCallback);
    dWorldStep (world,0.05);

    // remove all contact joints
    dJointGroupEmpty (contactgroup);
  }

  dsSetColor (0,1,1);
  dsSetTexture (DS_WOOD);
  dReal sides[3] = {LENGTH,WIDTH,HEIGHT};
  dsDrawBox (dBodyGetPosition(body[0]),dBodyGetRotation(body[0]),sides);
  dsSetColor (1,1,1);
  for (i=1; i<=4; i++) dsDrawCylinder (dBodyGetPosition(body[i]),
				       dBodyGetRotation(body[i]),0.04f,RADIUS);

	dVector3 ss;
	dGeomBoxGetLengths (ground_box,ss);
	dsDrawBox (dGeomGetPosition(ground_box),dGeomGetRotation(ground_box),ss);
	dGeomBoxGetLengths (ground_box2,ss);
	dsDrawBox (dGeomGetPosition(ground_box2),dGeomGetRotation(ground_box2),ss);
  

  /*
  printf ("%.10f %.10f %.10f %.10f\n",
	  dJointGetHingeAngle (joint[1]),
	  dJointGetHingeAngle (joint[2]),
	  dJointGetHingeAngleRate (joint[1]),
	  dJointGetHingeAngleRate (joint[2]));
  */
  
	float xyz[3];
	float hpr[3];
	dsGetViewpoint(xyz,hpr); 
	
  
	const dReal *a = dBodyGetPosition (body[0]);
	const dReal *b = dBodyGetRotation (body[0]);
	
			
	float theta = -asinf(b[8]);
	float phi = atan2f(b[4]/cos(theta),b[0]/cos(theta));
	//printf("phi: %f\n",phi);
	
	xyz[0] = a[0] - FOLLOW*cos(phi);
	xyz[1] = a[1] - FOLLOW*sin(phi);
	
	hpr[0] = phi/(2*M_PI)*360;
	
	
	dsSetViewpoint(xyz,hpr); 
}

void ProcessMessages() {
	printf("Thread Started\n");
	
	char readbuf[150];
	
	while(Run) {

		bzero(readbuf,sizeof(readbuf));

		fgets(readbuf,150,RXpipe); // Block here waiting for a message.
	
		std::string Message = readbuf;
		
		boost::algorithm::trim(Message);

		std::cout << "IPC - Received" << Message << "\n";

		std::vector<std::string> MessageParts;

		boost::algorithm::split(MessageParts, Message,  boost::algorithm::is_any_of(","));

		if(MessageParts[0].compare(0,1,"T") == 0) {
			speed = atof(MessageParts[1].c_str());
			printf("Set speed to %f\n", speed);
		}
		if(MessageParts[0].compare(0,1,"S") == 0) {
			steer = 0.00272707*atof(MessageParts[1].c_str());
			printf("Set steering to %f\n", steer);
		}
		if(MessageParts[0].compare(0,3,"POS") == 0) {
			const dReal *a = dBodyGetPosition (body[0]);
			const dReal *b = dBodyGetRotation (body[0]);
			printf("Current Position:\nx:%f\ny:%f\nz:%f\n",a[0],a[1],a[2]);
			
			float theta = -asinf(b[8]);
			float phi = atan2f(b[4]/cos(theta),b[0]/cos(theta))/(2*M_PI)*360;
			float bearing;
			if (phi < 0) {
				bearing = -phi;
			} else {
				bearing = 360 - phi;
			}
			printf("Current Bearing:%f\n",bearing);
			
			const dReal *c = dBodyGetLinearVel (body[0]);
			printf("Current Velocity:\nx:%f\ny:%f\nz:%f\n",c[0],c[1],c[2]);
			float speed = sqrtf(powf(c[0],2)+powf(c[1],2)+powf(c[2],2));
			printf("Current Speed:%f\n",speed);
		}		

	}

}

void sendPOS() {
	printf("Thread Started\n");
	
	while (true) {
		//printf("Running sendPOS");
		const dReal *a = dBodyGetPosition (body[0]);
		const dReal *b = dBodyGetRotation (body[0]);
		fprintf(TXpipe,"P,%f,%f,%f\n",a[0],a[1],a[2]);
			
		float theta = -asinf(b[8]);
		float phi = atan2f(b[4]/cos(theta),b[0]/cos(theta))/(2*M_PI)*360;
		float bearing;
		if (phi >= -180 && phi <=90) {
			bearing = abs(phi-90);
		} else {
			bearing = 360 - (phi - 90);
		}
		fprintf(TXpipe,"B,%f\n",bearing);
		
		const dReal *c = dBodyGetLinearVel (body[0]);
		//fprintf(TXpipe,"Current Velocity:\nx:%f\ny:%f\nz:%f\n",c[0],c[1],c[2]);
		float speed = sqrtf(powf(c[0],2)+powf(c[1],2)+powf(c[2],2));
		fprintf(TXpipe,"S,%f\n",speed);
		fflush(TXpipe);
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

}


int main (int argc, char **argv)
{
  int i;
  dMass m;

  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = &command;
  fn.stop = 0;
  fn.path_to_textures = DRAWSTUFF_TEXTURE_PATH;

  // create world
  dInitODE2(0);
  world = dWorldCreate();
  space = dHashSpaceCreate (0);
  contactgroup = dJointGroupCreate (0);
  dWorldSetGravity (world,0,0,-0.5);
  ground = dCreatePlane (space,0,0,1,0);

  // chassis body
  body[0] = dBodyCreate (world);
  dBodySetPosition (body[0],0,0,STARTZ);
  dMassSetBox (&m,1,LENGTH,WIDTH,HEIGHT);
  dMassAdjust (&m,CMASS);
  dBodySetMass (body[0],&m);
  box[0] = dCreateBox (0,LENGTH,WIDTH,HEIGHT);
  dGeomSetBody (box[0],body[0]);

  // wheel bodies
  for (i=1; i<=4; i++) {
    body[i] = dBodyCreate (world);
    dQuaternion q;
    dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
    dBodySetQuaternion (body[i],q);
    dMassSetSphere (&m,1,RADIUS);
    dMassAdjust (&m,WMASS);
    dBodySetMass (body[i],&m);
    sphere[i-1] = dCreateSphere (0,RADIUS);
    dGeomSetBody (sphere[i-1],body[i]);
  }
  dBodySetPosition (body[1],0.5*LENGTH,WIDTH*0.5,STARTZ-HEIGHT*0.5);
  dBodySetPosition (body[2],0.5*LENGTH,-WIDTH*0.5,STARTZ-HEIGHT*0.5);
  dBodySetPosition (body[3],-0.5*LENGTH, WIDTH*0.5,STARTZ-HEIGHT*0.5);
  dBodySetPosition (body[4],-0.5*LENGTH,-WIDTH*0.5,STARTZ-HEIGHT*0.5);
  

  // front wheel hinge
  /*
  joint[0] = dJointCreateHinge2 (world,0);
  dJointAttach (joint[0],body[0],body[1]);
  const dReal *a = dBodyGetPosition (body[1]);
  dJointSetHinge2Anchor (joint[0],a[0],a[1],a[2]);
  dJointSetHinge2Axis1 (joint[0],0,0,1);
  dJointSetHinge2Axis2 (joint[0],0,1,0);
  */

  // front and back wheel hinges
  for (i=0; i<4; i++) {
    joint[i] = dJointCreateHinge2 (world,0);
    dJointAttach (joint[i],body[0],body[i+1]);
    const dReal *a = dBodyGetPosition (body[i+1]);
    dJointSetHinge2Anchor (joint[i],a[0],a[1],a[2]);
    dJointSetHinge2Axis1 (joint[i],0,0,1);
    dJointSetHinge2Axis2 (joint[i],0,1,0);
  }

  // set joint suspension
  for (i=0; i<4; i++) {
    dJointSetHinge2Param (joint[i],dParamSuspensionERP,0.4);
    dJointSetHinge2Param (joint[i],dParamSuspensionCFM,0.8);
  }

  // lock back wheels along the steering axis
  for (i=2; i<4; i++) {
    // set stops to make sure wheels always stay in alignment
    dJointSetHinge2Param (joint[i],dParamLoStop,0);
    dJointSetHinge2Param (joint[i],dParamHiStop,0);
    // the following alternative method is no good as the wheels may get out
    // of alignment:
    //   dJointSetHinge2Param (joint[i],dParamVel,0);
    //   dJointSetHinge2Param (joint[i],dParamFMax,dInfinity);
  }

  // create car space and add it to the top level space
  car_space = dSimpleSpaceCreate (space);
  dSpaceSetCleanup (car_space,0);
  dSpaceAdd (car_space,box[0]);
  dSpaceAdd (car_space,sphere[0]);
  dSpaceAdd (car_space,sphere[1]);
  dSpaceAdd (car_space,sphere[2]);
  dSpaceAdd (car_space,sphere[3]);

  // environment
  ground_box = dCreateBox (space,1,1,3);
  ground_box2 = dCreateBox (space,1,1,3);
  dGeomSetPosition (ground_box,30,70,1.5);
  dGeomSetPosition (ground_box2,18,64,1.5);
  //62
  
  // set up RX_IPC
  mode_t process_mask = umask(0);
  int res = mkfifo("./SIM_IPC_FIFO_RX", S_IRWXU | S_IRWXG | S_IRWXO);
  umask(process_mask);
  if(res != 0) { printf("IPC - Couldn't create fifo!\n"); }
  
  OpenState = true;
  RXpipe = fopen("./SIM_IPC_FIFO_RX", "r+");
  if(RXpipe == NULL) { printf("IPC - RX Pipe open failed...\n"); OpenState = false; }
  else { printf("IPC - RX Pipe open okay.\n"); }
  printf("Waiting for car code....\n");
  
  TXpipe = fopen("/home/martinfrench/SAEAuto/Control/CAR_IPC_FIFO_RX", "w");
  if(TXpipe == NULL) { printf("IPC - TX Pipe open failed...\n"); OpenState = false; }
  else { printf("IPC - TX Pipe open okay.\n"); }
  
  //fprintf(TXpipe,"Test");
  //fflush(TXpipe);
  
  if(OpenState) {	

		m_Thread = boost::thread(&ProcessMessages);
		m_Thread.detach();
		
		pos_Thread = boost::thread(&sendPOS);
		pos_Thread.detach();

  }

  // run simulation
  Run = true;
  dsSimulationLoop (argc,argv,352,288,&fn);

  dGeomDestroy (box[0]);
  dGeomDestroy (sphere[0]);
  dGeomDestroy (sphere[1]);
  dGeomDestroy (sphere[2]);
  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (space);
  dWorldDestroy (world);
  dCloseODE();
  return 0;
}
