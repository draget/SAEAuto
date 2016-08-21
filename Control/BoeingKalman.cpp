#ifndef KALMAN_H
#define KALMAN_H

/** OpenCV Kalman filter for 1st, 2nd and 3rd order position, velocity and acceleration data.
    (c) Adrian Boeing
    www.adrianboeing.com
 */
#include <opencv2/opencv.hpp>

typedef enum {
    K_P = 1, //position only
    K_V = 2, //velocity only
    K_A = 4, //acceleration only
    K_PV = 3, //position & velocity
    K_PA = 5, //position & acceleration
    K_VA = 6, //velocity & acceleration
    K_PVA = 7 //position, velocity and acceleration
} Kalman_e;

class KalmanPVA {
public:
    KalmanPVA(int order, Kalman_e type, float initial_pos, float initial_vel, float initial_accel, float dt, float process_noise[3], float measurement_noise[3]) {
        m_order = order;
        //create the OpenCV kalman structures
        kalman = cvCreateKalman(order, 3, 0 );
        z = cvCreateMat(3,1,CV_32FC1);
        
        //construct the transition matrix
        SetDT(dt);
        
        //enable only the measurements we have!
        for (int i=0;i<m_order;i++) {
            cvmSet(kalman->measurement_matrix,i,i,((type&7)>>i)&1);
        }
        //configure the process noise matrix
        cvSetIdentity( kalman->process_noise_cov,    cvRealScalar(1) ); //zero the matrix 
        for (int i=0;i<m_order;i++) {
            cvmSet( kalman->process_noise_cov,i,i, process_noise[i] ); 
        }
        //configure the measurment noise matrix
        cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1) ); //zero the matrix
        for (int i=0;i<3;i++) {
            cvmSet(kalman->measurement_noise_cov,i,i,measurement_noise[i]);
        }
        cvSetIdentity( kalman->error_cov_post,cvRealScalar(10) ); //large initial unknown
        
        //setup the initial state
        cvmSet(kalman->state_post,0,0,initial_pos );
        if (m_order>1)
            cvmSet(kalman->state_post,1,0,initial_vel);
        if (m_order>2)
            cvmSet(kalman->state_post,2,0,initial_accel);
        
        
    };
    void SetDT(float dt) {
        //3rd, 2nd, and 1st order transition matrix models
        
        //3rd order uses 2nd order leapfrog integration
        float F3[] = {   1, dt, 0.5*dt*dt,
            0,  1, dt,
            0,  0,  1};
        
        float F2[] = {   1, dt, 
            0,  1};
        
        float F1[] = {1};
        
        switch (m_order) {
            case 3:
                memcpy( kalman->transition_matrix->data.fl, F3, sizeof(F3));
                break;
            case 2:
                memcpy( kalman->transition_matrix->data.fl, F2, sizeof(F2));
                break;
            case 1:
                memcpy( kalman->transition_matrix->data.fl, F1, sizeof(F1));
                break;
        }
    }
    void SetMeasurements(float pos, float vel, float accel) {
        cvmSet(z,0,0,pos);
        //if (z->rows>1)
            cvmSet(z,1,0,vel);
        //if (z->rows>2)
            cvmSet(z,2,0,accel);
    }
    
    void Update(){
        const CvMat* y = cvKalmanPredict( kalman, 0 );
        cvKalmanCorrect( kalman, z );
    }
    float GetPositionEstimate() {
        float v = cvmGet(kalman->state_post,0,0);
        return v;
    }
    float GetVelocityEstimate() {
        if (kalman->state_post->rows>1)
            return cvmGet(kalman->state_post,1,0);
        return 0;
    }
    float GetAccelerationEstimate() {
        if (kalman->state_post->rows>2)
            return cvmGet(kalman->state_post,2,0);
        return 0;
    }
    float GetPositionError() {
        return cvmGet(kalman->error_cov_post,0,0);
    }
    CvKalman* kalman; //openCV Kalman filter
    CvMat* z; //measurement matrix
private:

    void cvKalmanNoObservation() { 
        cvCopy(kalman->error_cov_pre, kalman->error_cov_post); 
        cvCopy(kalman->state_pre, kalman->state_post); 
    } 
    int m_order;
    
};

#endif
