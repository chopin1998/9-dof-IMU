
#include "tilt.h"


void tilt_init(tilt *self, float dt, float R_angle, float Q_gyro, float Q_angle)
{
    self->bias = 0.0;
    self->rate = 0.0;
    self->angle = 0.0;

    self->dt = dt;

    self->R_angle = R_angle;

    self->Q_gyro = Q_gyro;
    self->Q_angle = Q_angle;

    self->P_00 = 1.0;
    self->P_01 = 0.0;
    self->P_10 = 0.0;
    self->P_11 = 1.0;
}

void tilt_state_update(tilt *self, float gyro_rate)
{
    static float gyro_rate_unbiased;
    static float Pdot_00;
    static float Pdot_01;
    static float Pdot_10;
    static float Pdot_11;

    gyro_rate_unbiased = gyro_rate - self->bias;

    Pdot_00 = self->Q_angle - self->P_01 - self->P_10;
    Pdot_01 = -self->P_11;
    Pdot_10 = -self->P_11;
    Pdot_11 = self->Q_gyro;

    self->rate = gyro_rate_unbiased;

    self->angle += gyro_rate_unbiased * self->dt;
    
    self->P_00 += Pdot_00 * self->dt;
    self->P_01 += Pdot_01 * self->dt;
    self->P_10 += Pdot_10 * self->dt;
    self->P_11 += Pdot_11 * self->dt;
}

void tilt_kalman_update(tilt *self, float angle_measured)
{
    static float angle_error;
    static float C_0;
    static float PCt_0;
    static float PCt_1;
    static float E;
    static float K_0;
    static float K_1;
    static float t_0;
    static float t_1;

    angle_error = angle_measured - self->angle;

    C_0 = 1.0;

    PCt_0 = C_0 * self->P_00; /* + C_1 * P_01 = 0 */
    PCt_1 = C_0 * self->P_10; /* + C_1 * P_11 = 0 */

    E = self->R_angle + C_0 * PCt_0; /* + C_1 * PCt_1 = 0 */

    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;

    t_0 = PCt_0; /* C_0 * P[0][0] + C_1 * P[1][0] */
    t_1 = C_0 * self->P_01; /* + C_1 * P[1][1]  = 0 */

    self->P_00 -= K_0 * t_0;
    self->P_01 -= K_0 * t_1;
    self->P_10 -= K_1 * t_0;
    self->P_11 -= K_1 * t_1;

    self->bias  += K_1 * angle_error;
    self->angle += K_0 * angle_error;
}

