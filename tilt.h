
#ifndef _IMU_TILT_H_
#define _IMU_TILT_H_ 1

typedef struct _tilt tilt;

struct _tilt
{
    float bias;
    float rate;
    float angle;

    float P_00;
    float P_01;
    float P_10;
    float P_11;

    float dt;
    float R_angle;
    float Q_gyro;
    float Q_angle;
};

void tilt_init(tilt *self, float dt, float R_angle, float Q_gyro, float Q_angle);
void tilt_state_update(tilt *self, float gyro_rate);
void tilt_kalman_update(tilt *self, float angle_measured);

inline static float tilt_get_bias(tilt *self)
// Get the bias.
{
    return self->bias;
}

inline static float tilt_get_rate(tilt *self)
// Get the rate.
{
    return self->rate;
}

inline static float tilt_get_angle(tilt *self)
// Get the angle.
{
    return self->angle;
}

#endif // _IMU_TILT_H_
