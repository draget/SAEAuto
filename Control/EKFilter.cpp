////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      GU Zhaopeng <zpgu@nlpr.ia.ac.cn>
// Version:     0.0.1
// Date:        2009-12                                         
// Description:   A simple & clear OpenCV based Extended Kalman Filter abstract class implementation,
//                absolutely following standard EKF equations. Special thanks to the opensource project of
//                KFilter1.3. The code is still being tested，so I'll do the best to make sure the safety of them. 
//
//
// Copyright (C) 2009-2010 OpenPR 
// All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
// 
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer. 
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions and the following disclaimer in the 
//       documentation and/or other materials provided with the distribution. 
//     * Neither the name of OpenPR nor the names of its  
//       contributors may be used to endorse or promote products derived 
//       from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL HOLDER AND CONTRIBUTORS BE LIABLE FOR ANY 
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EKFilter.h"
#include <iostream>
#include <fstream>
using namespace std;

EKFilter::EKFilter() {
	
	//Dimension of State Vector
	kalman_x_dimension=0;
	//Dimension of Measurement Vector
	kalman_z_dimension=0;
	//Dimension of Control Vector
	kalman_u_dimension=0;
	//Dimension of Process Noise Vector
	kalman_w_dimension=0;
	//Dimension of Measurement Noise Vector
	kalman_v_dimension=0;

	//State Vector
	kalman_x_last=NULL;
	kalman_x_predicted=NULL;
	kalman_x_updated=NULL;
	//Control Vector
	kalman_u=NULL;
	//Measurement Vector
	kalman_z=NULL;
	kalman_z_predicted=NULL;

	////Matrices
	kalman_P_last=NULL;
	kalman_P_predicted=NULL;
	kalman_P_updated=NULL;
	kalman_A=NULL;
	kalman_W=NULL;
	kalman_Q=NULL;
	kalman_H=NULL;
	kalman_V=NULL;
	kalman_R=NULL;
	kalman_S=NULL;
	kalman_K=NULL;

}

EKFilter::~EKFilter()
{
	ClearMatrices();
}

int EKFilter::ClearMatrices()
{
	if (kalman_x_last!=NULL)
		kalman_x_last.release():
	if (kalman_x_predicted!=NULL)
		kalman_x_predicted.release();
	if (kalman_x_updated!=NULL)
		kalman_x_updated.release();
	if (kalman_u!=NULL)
		kalman_u.release();
	if (kalman_z!=NULL)
		kalman_z.release();
	if (kalman_z_predicted!=NULL)
		kalman_z_predicted.release();
	if (kalman_P_last!=NULL)
		kalman_P_last.release();
	if (kalman_P_predicted!=NULL)
		kalman_P_predicted.release();
	if (kalman_P_updated!=NULL)
		kalman_P_updated.release();
	if (kalman_A!=NULL)
		kalman_A.release();
	if (kalman_W!=NULL)
		kalman_W.release();
	if (kalman_Q!=NULL)
		kalman_Q.release();
	if (kalman_H!=NULL)
		kalman_H.release();
	if (kalman_V!=NULL)
		kalman_V.release();
	if (kalman_R!=NULL)
		kalman_R.release();
	if (kalman_S!=NULL)
		kalman_S.release();
	if (kalman_K!=NULL)
		kalman_K.release();

	return 0;
}

int EKFilter::SetDimensions(int x_dimension, int u_dimension, int z_dimension, int w_dimension, int v_dimension)
{
	kalman_x_dimension=x_dimension;
	kalman_u_dimension=u_dimension;
	kalman_z_dimension=z_dimension;
	kalman_w_dimension=w_dimension;
	kalman_v_dimension=v_dimension;

	return 0;
}

int EKFilter::CheckDimension(Mat M1,Mat M2)
{
	if (M1==NULL || M2==NULL)
		return -1;
	if (M1->cols!=M2->cols ||M1->rows!=M2->rows)
		return -1;

	return 0;
}

int EKFilter::InitMatrices()
{
	ClearMatrices();
	
	//State Vector
	kalman_x_last.create(kalman_x_dimension,1,CV_32F);
	kalman_x_last = Mat::zeros(kalman_x_dimension,1,CV_32F);
	kalman_x_predicted.create(kalman_x_dimension,1,CV_32F);
	kalman_x_predicted = Mat::zeros(kalman_x_dimension,1,CV_32F);
	kalman_x_updated.create(kalman_x_dimension,1,CV_32F);
	kalman_x_updated = Mat::zeros(kalman_x_dimension,1,CV_32F);
	//Control Vector
	kalman_u.create(kalman_u_dimension,1,CV_32F);
	kalman_u = Mat::zeros(kalman_u_dimension,1,CV_32F);
	//Measurement Vector
	kalman_z.create(kalman_z_dimension,1,CV_32F);
	kalman_z = Mat::zeros(kalman_z_dimension,1,CV_32F);
	kalman_z_predicted.create(kalman_z_dimension,1,CV_32F);
	kalman_z_predicted = Mat::zeros(kalman_z_dimension,1,CV_32F);

	////Matrices
	kalman_P_last.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_P_last = Mat::zeros(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_P_predicted.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_P_predicted = Mat::zeros(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_P_updated.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_P_updated = Mat::zeros(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_F.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_F = Mat::zeros(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_f.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_f = Mat::zeros(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_W.create(kalman_x_dimension,kalman_w_dimension,CV_32F);
	kalman_W = Mat::zeros(kalman_x_dimension,kalman_w_dimension,CV_32F);
	kalman_Q.create(kalman_w_dimension,kalman_w_dimension,CV_32F);
	kalman_Q = Mat::zeros(kalman_w_dimension,kalman_w_dimension,CV_32F);
	kalman_H.create(kalman_z_dimension,kalman_x_dimension,CV_32F);
	kalman_H = Mat::zeros(kalman_z_dimension,kalman_x_dimension,CV_32F);
	kalman_h.create(kalman_z_dimension,kalman_x_dimension,CV_32F);
	kalman_h = Mat::zeros(kalman_z_dimension,kalman_x_dimension,CV_32F);
	kalman_V.create(kalman_z_dimension,kalman_v_dimension,CV_32F);
	kalman_V = Mat::zeros(kalman_z_dimension,kalman_v_dimension,CV_32F);
	kalman_R.create(kalman_v_dimension,kalman_v_dimension,CV_32F);
	kalman_R = Mat::zeros(kalman_v_dimension,kalman_v_dimension,CV_32F);
	kalman_S.create(kalman_z_dimension,kalman_z_dimension,CV_32F);
	kalman_S = Mat::zeros(kalman_z_dimension,kalman_z_dimension,CV_32F);
	kalman_K.create(kalman_x_dimension,kalman_z_dimension,CV_32F);
	kalman_K = Mat::zeros(kalman_x_dimension,kalman_z_dimension,CV_32F);

	return 0;
}

int EKFilter::Set_NoiseCovariance(Mat Q,Mat R) {
	
	if (Q.empty() || R.empty())
		return -1;
	R.copyTo(kalman_R);
	Q.copyTo(kalman_Q);
	return 0;
	
}

int EKFilter::PredictionStep() {
	Get_x_predicted();
	Get_F();
	Get_W();
	Get_H();
	Get_V();
	kalman_x_predicted.copyTo(kalman_x_last);
	return 0;
}

int EKFilter::Get_x_predicted(float dt) {
		
	kalman_x_predicted.at<double>(0,0) = kalman_x_last.at<double>(0,0) + cos(kalman_x_last.at<double>(2,0))*(dt*kalman_x_last.at<double>(3,0) + dt*dt*kalman_x_last.at<double>(4,0)/2);
	kalman_x_predicted.at<double>(1,0) = kalman_x_last.at<double>(1,0) + sin(kalman_x_last.at<double>(2,0))*(dt*kalman_x_last.at<double>(3,0) + dt*dt*kalman_x_last.at<double>(4,0)/2);
	kalman_x_predicted.at<double>(2,0) = kalman_x_last.at<double>(2,0);
	kalman_x_predicted.at<double>(3,0) = kalman_x_last.at<double>(3,0) + dt*kalman_x_last.at<double>(4,0);
	kalman_x_predicted.at<double>(4,0) = kalman_x_last.at<double>(4,0);

	Get_F(dt);
	
	return 0;
	
}

int EKFilter::Get_F(float dt)
{
	kalman_F = Mat::eye(kalman_x_dimension,kalman_x_dimension,CV_32F);
	kalman_F.at<double>(0,2) = -1*sin(kalman_x_last.at<double>(2,0))*(dt*kalman_x_last.at<double>(3,0) + dt*dt*kalman_x_last.at<double>(4,0)/2);
	kalman_F.at<double>(0,3) = dt*cos(kalman_x_last.at<double>(2,0));
	kalman_F.at<double>(0,4) = dt*dt*cos(kalman_x_last.at<double>(2,0))/2;
	kalman_F.at<double>(1,2) = cos(kalman_x_last.at<double>(2,0))*(dt*kalman_x_last.at<double>(3,0) + dt*dt*kalman_x_last.at<double>(4,0)/2);
	kalman_F.at<double>(1,3) = dt*sin(kalman_x_last.at<double>(2,0));
	kalman_F.at<double>(1,4) = dt*dt*sin(kalman_x_last.at<double>(2,0))/2;
	kalman_F.at<double>(3,4) = dt;
	
	return 0;
}

int EKFilter::Get_W()
{
	kalman_W = Mat::eye(kalman_x_dimension,kalman_w_dimension,CV_32F);
	return 0;
}

int EKFilter::Get_P_predicted()
{
	Mat kalman_Ft.create(kalman_x_dimension,kalman_x_dimension,CV_32F);
	transpose(kalman_F,kalman_Ft);
	Mat kalman_Wt.create(kalman_w_dimension,kalman_x_dimension,CV_32F);
	transpose(kalman_W,kalman_Wt);
	kalman_P_predicted = kalman_F * kalman_P_last * kalman_Ft + kalman_W * kalman_Q * kalman_Wt;
	
	/*
	Mat kalman_At.create(kalman_A->cols,kalman_A->rows,CV_32F);
	Mat kalman_Wt.create(kalman_W->cols,kalman_W->rows,CV_32F);
	Mat kalman_A_x_P.create(kalman_A->rows,kalman_P_last->cols,CV_32F);
	Mat kalman_A_x_P_x_At.create(kalman_A->rows,kalman_A->rows,CV_32F);
	Mat kalman_W_x_Q.create(kalman_W->rows,kalman_Q->cols,CV_32F);
	Mat kalman_W_x_Q_x_Wt.create(kalman_W->rows,kalman_W->rows,CV_32F);

	cvTranspose(kalman_A,kalman_At);
	cvTranspose(kalman_W,kalman_Wt);
	MatMul(kalman_A,kalman_P_last,kalman_A_x_P);
	MatMul(kalman_A_x_P,kalman_At,kalman_A_x_P_x_At);
	MatMul(kalman_W,kalman_Q,kalman_W_x_Q);
	MatMul(kalman_W_x_Q,kalman_Wt,kalman_W_x_Q_x_Wt);

	cvAdd(kalman_A_x_P_x_At,kalman_W_x_Q_x_Wt,kalman_P_predicted);

	cvReleaseMat(&kalman_At);
	cvReleaseMat(&kalman_Wt);
	cvReleaseMat(&kalman_A_x_P);
	cvReleaseMat(&kalman_A_x_P_x_At);
	cvReleaseMat(&kalman_W_x_Q);
	cvReleaseMat(&kalman_W_x_Q_x_Wt);
	*/
	return 0;
}

int EKFilter::Set_z(Mat* z)
{
	if (CheckDimension(z,kalman_z)!=0)
		return -1;
	//cvmCopy(z,kalman_z);
	cvReleaseMat(&kalman_z);
	kalman_z=cvCloneMat(z);
	return 0;
}

//int EKFilter::Get_z_predicted()
//{
//	return 0;
//}

int EKFilter::Get_H()
{
	
	cvSetZero(kalman_H);
	return 0;
}

int EKFilter::Get_S()
{
	////Debug 
	//OutputMat(kalman_V);

	Mat* kalman_Ht.create(kalman_H->cols,kalman_H->rows,CV_32F);
	Mat* kalman_Vt.create(kalman_V->cols,kalman_V->rows,CV_32F);
	Mat* kalman_H_x_P.create(kalman_H->rows,kalman_P_predicted->cols,CV_32F);
	Mat* kalman_H_x_P_x_Ht.create(kalman_H->rows,kalman_H->rows,CV_32F);
	Mat* kalman_V_x_R.create(kalman_V->rows,kalman_R->cols,CV_32F);
	Mat* kalman_V_x_R_x_Vt.create(kalman_V->rows,kalman_V->rows,CV_32F);

	cvTranspose(kalman_H,kalman_Ht);
	cvTranspose(kalman_V,kalman_Vt);
	MatMul(kalman_H,kalman_P_predicted,kalman_H_x_P);
	MatMul(kalman_H_x_P,kalman_Ht,kalman_H_x_P_x_Ht);
	MatMul(kalman_V,kalman_R,kalman_V_x_R);
	MatMul(kalman_V_x_R,kalman_Vt,kalman_V_x_R_x_Vt);

	cvAdd(kalman_H_x_P_x_Ht,kalman_V_x_R_x_Vt,kalman_S);

	cvReleaseMat(&kalman_Ht);
	cvReleaseMat(&kalman_Vt);
	cvReleaseMat(&kalman_H_x_P);
	cvReleaseMat(&kalman_H_x_P_x_Ht);
	cvReleaseMat(&kalman_V_x_R);
	cvReleaseMat(&kalman_V_x_R_x_Vt);

	return 0;
}

int EKFilter::Get_K()
{

	Mat* kalman_Ht.create(kalman_H->cols,kalman_H->rows,CV_32F);
	Mat* kalman_Sinv.create(kalman_S->cols,kalman_S->rows,CV_32F);
	Mat* kalman_P_x_Ht.create(kalman_P_predicted->rows,kalman_Ht->cols,CV_32F);

	cvTranspose(kalman_H,kalman_Ht);
	cvInvert(kalman_S,kalman_Sinv);
	MatMul(kalman_P_predicted,kalman_Ht,kalman_P_x_Ht);
	MatMul(kalman_P_x_Ht,kalman_Sinv,kalman_K);

	cvReleaseMat(&kalman_Sinv);
	cvReleaseMat(&kalman_Ht);
	cvReleaseMat(&kalman_P_x_Ht);

	return 0;
}

int EKFilter::Get_x_updated()
{
	Mat* kalman_z_minus_z_predicted.create(kalman_z_dimension,1,CV_32F);
	Mat* kalman_K_x_z_minus_z_predicted.create(kalman_K->rows,1,CV_32F);

	cvmSub(kalman_z,kalman_z_predicted,kalman_z_minus_z_predicted);
	MatMul(kalman_K,kalman_z_minus_z_predicted,kalman_K_x_z_minus_z_predicted);
	cvmAdd(kalman_x_predicted,kalman_K_x_z_minus_z_predicted,kalman_x_updated);

	cvReleaseMat(&kalman_z_minus_z_predicted);
	cvReleaseMat(&kalman_K_x_z_minus_z_predicted);

	return 0;
}

int EKFilter::Get_P_updated()
{
	Mat* kalman_I.create(kalman_P_predicted->rows,kalman_P_predicted->rows,CV_32F);
	Mat* kalman_K_x_H.create(kalman_K->rows,kalman_H->cols,CV_32F);
	Mat* kalman_I_minus_K_x_H.create(kalman_K_x_H->rows,kalman_K_x_H->cols,CV_32F);

	MatMul(kalman_K,kalman_H,kalman_K_x_H);
	cvSetIdentity(kalman_I);
	cvmSub(kalman_I,kalman_K_x_H,kalman_I_minus_K_x_H);
	MatMul(kalman_I_minus_K_x_H,kalman_P_predicted,kalman_P_updated);

	//Make sure kalman_P_updated is symmetric
	for (int row=0;row<kalman_P_updated->rows;row++)
	{
		for (int col=row;col<kalman_P_updated->cols;col++)
		{
			double tmp;
			tmp=cvmGet(kalman_P_updated,row,col)+cvmGet(kalman_P_updated,col,row);
			tmp=tmp/2;
			cvmSet(kalman_P_updated,row,col,tmp);
			cvmSet(kalman_P_updated,col,row,tmp);
		}
	}
	//Mat* P_half.create(kalman_P_updated->rows,kalman_P_updated->cols,CV_32F);
	//Mat* P_half_t.create(kalman_P_updated->cols,kalman_P_updated->rows,CV_32F);

	//for (int row=0;row<kalman_P_updated->rows;row++)
	//{
	//	for (int col=0;col<kalman_P_updated->cols;col++)
	//	{
	//		cvmSet(P_half,row,col,0.5*cvmGet(kalman_P_updated,row,col));
	//	}
	//}

	//cvTranspose(P_half,P_half_t);

	//cvAdd(P_half,P_half_t,kalman_P_updated);

	////Debug
	//OutputMat(kalman_K);cout<<endl;
	//OutputMat(kalman_H);cout<<endl;
	//OutputMat(kalman_I);cout<<endl;
	//OutputMat(kalman_I_minus_K_x_H);cout<<endl;
	//OutputMat(kalman_P_updated);cout<<endl;

	//cvReleaseMat(&P_half);
	//cvReleaseMat(&P_half_t);

	cvReleaseMat(&kalman_I);
	cvReleaseMat(&kalman_K_x_H);
	cvReleaseMat(&kalman_I_minus_K_x_H);

	return 0;
}

int EKFilter::OutputMat(Mat* M,int order)
{
	if (order==0)
		for (int row=0;row<M->rows;row++)
		{
			for (int col=0;col<M->cols;col++)
			{
				cout<<cvmGet(M,row,col)<<" ";
			}
			cout<<endl;
		}
	else
		for (int col=0;col<M->cols;col++)
		{
			for (int row=0;row<M->rows;row++)
			{
				cout<<cvmGet(M,row,col)<<" ";
			}
			cout<<endl;
		}
	return 0;
}
