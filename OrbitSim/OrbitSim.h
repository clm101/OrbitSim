#include "Application.h"

#ifndef ORBITSIM_H
#define ORBITSIM_H

class OrbitSim : public Application {
private:
	GFX::Circle c;
public:
	OrbitSim(const wchar_t*, short, short);
	OrbitSim(const Application&) = delete;
	OrbitSim& operator=(const Application) = delete;

	void run(const float) override;
};

#endif