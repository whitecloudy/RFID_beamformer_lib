#ifndef __KALMAN_FILTER__
#define __KALMAN_FILTER__

#include <iostream>
#include <armadillo>

class Kalman_filter {
  private:
    int N;
    arma::cx_mat  x_k, P_k, Q, R;
    arma::cx_mat  x_k_, P_k_;
    arma::cx_mat  K_k;

    int prediction_update(arma::cx_mat A);
    int kalman_gain_update(arma::cx_mat H);
    int measurement_update(arma::cx_mat H, arma::cx_mat z_k);
    int covariance_update(arma::cx_mat H);

  public:
    Kalman_filter(int matrix_size, arma::cx_mat x0, arma::cx_mat P0, arma::cx_mat Q, arma::cx_mat R);

    arma::cx_mat process(arma::cx_mat z, arma::cx_mat H, arma::cx_mat A);
};

#endif
