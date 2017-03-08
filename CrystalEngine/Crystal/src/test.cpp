#include<iostream>
#include<crystal/pfgen.h>

using namespace crystal;
/**
int main()
{
	
	Particle p;
	Vector3 po{ 0,0,0 };
	p.setPosition(po);
	p.setMass(1);
	p.damping = 0.9999;

	ParticleBuoyancy buoyancy{ 0.5f,1.0f,0};
	ParticleForceRegistry registry;
	registry.add(&p, &buoyancy);
	registry.updateForces(2);

	p.integrate(2);

	std::cout << p.getVelocity().y << std::endl;
	
	system("pause");
	
}
*/