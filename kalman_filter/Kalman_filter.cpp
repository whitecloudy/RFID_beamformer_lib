#include "Kalman_filter.hpp"


//initializer
Kalman_filter::Kalman_filter(int matrix_size, arma::cx_mat x0, arma::cx_mat P0, arma::cx_mat Q, arma::cx_mat R)
{
  this->N = matrix_size;

  this->x_k = x0;
  this->P_k = P0;
  this->Q = Q;
  this->R = R;
}

//
//Private Function
//
int Kalman_filter::prediction_update(arma::cx_mat A)
{
  x_k_ = A*x_k;
  P_k_ = A*P_k*A.t() + Q;

  return 0;
}


int Kalman_filter::kalman_gain_update(arma::cx_mat H)
{
  K_k = P_k_*H.t()*(H*P_k_*H.t()+R).i();

  return 0;
}


int Kalman_filter::measurement_update(arma::cx_mat H, arma::cx_mat z_k)
{
  x_k = x_k_ + K_k*(z_k - H*x_k_);

  return 0;
}

int Kalman_filter::covariance_update(arma::cx_mat H)
{
  P_k = P_k_ - K_k*H*P_k_;

  return 0;
}

//
//public Function
//
arma::cx_mat Kalman_filter::process(arma::cx_mat z, arma::cx_mat H, arma::cx_mat A)
{
  prediction_update(A);  
  kalman_gain_update(H);
  measurement_update(H, z);
  covariance_update(H);

  return x_k;
}
