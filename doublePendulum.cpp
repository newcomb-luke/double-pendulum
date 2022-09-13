#include "doublePendulum.h"
#include <cmath>

const double GRAVITY = 9.81;

DoublePendulum::DoublePendulum() {
	_theta1 = 0.0;
	_theta2 = 0.0;
}

DoublePendulum::DoublePendulum(double theta1, double theta2) {
	_theta1 = theta1;
	_theta2 = theta2;
}

void DoublePendulum::step(double deltaT) {
	/*
	m_1, m_2 = self.m[:, 0], self.m[:, 1]
        t_1, t_2 = self.t[:, 0], self.t[:, 1]
        l_1, l_2 = self.l[:, 0], self.l[:, 1]
        t_d1, t_d2 = self.t_d[:, 0], self.t_d[:, 1]
        bottom = (2 * m_1) + m_2 - (m_2 * np.cos(2 * (t_1 - t_2)))
        theta_dd1 = (-self.g * (2 * m_1 + m_2) * np.sin(t_1) - m_2 * self.g * np.sin(t_1 - 2 * t_2) - 2 * np.sin(t_1 - t_2) * m_2 * (t_d2**2 * l_2 + t_d1**2 * l_1 * np.cos(t_1 - t_2))) / (l_1 * bottom)
        theta_dd2 = (2 * np.sin(t_1 - t_2) * (t_d1**2 * l_1 * (m_1 + m_2) + self.g * (m_1 + m_2) * np.cos(t_1) + t_d2**2 * l_2 * m_2 * np.cos(t_1 - t_2))) / (l_2 * bottom)
        self.t_d[:, 0] += delta_t * theta_dd1
        self.t_d[:, 1] += delta_t * theta_dd2
        self.t += delta_t * self.t_d
	*/
	double twoM1M2 = (2.0 * _mass1) + _mass2;
	double deltaTheta1Sq = pow(_deltaTheta1, 2.0);
	double deltaTheta2Sq = pow(_deltaTheta2, 2.0);
	double cosThetaDiff = cos(_theta1 - _theta2);
	double sinThetaDiff = sin(_theta1 - _theta2);
	double bottom = twoM1M2 - (_mass2 * cos(2.0 * (_theta1 - _theta2)));
	double ddTheta1 = (-GRAVITY * twoM1M2 * sin(_theta1) - _mass2 * GRAVITY * sin(_theta1 - 2.0 * _theta2) - 2.0 * sinThetaDiff * _mass2 * ( deltaTheta2Sq * _length2 + deltaTheta1Sq * _length1 * cosThetaDiff )) / (_length1 * bottom);
	double ddTheta2 = (2.0 * sinThetaDiff * (deltaTheta1Sq * _length1 * (_mass1 - _mass2) + GRAVITY * (_mass1 + _mass2) * cos(_theta1) + deltaTheta2Sq * _length2 * _mass2 * cosThetaDiff)) / (_length2 * bottom);
	_deltaTheta1 += deltaT * ddTheta1;
	_deltaTheta2 += deltaT * ddTheta2;
	_theta1 += deltaT * _deltaTheta1;
	_theta2 += deltaT * _deltaTheta2;
}

PendulumColor DoublePendulum::getColor() {
	const double radius = 127.0;

	double sinTheta1 = sin(_theta1);

	return PendulumColor {
		(int) (127.0 + radius * cos(_theta2) * sinTheta1),
		(int) (127.0 + radius * sin(_theta2) * sinTheta1),
		(int) (127.0 + radius * cos(_theta1))
	};
}
