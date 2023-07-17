#include "./Force.h"
#include <algorithm>

Vec2 Force::GenerateDragForce(const Particle& particle, float k) {
	Vec2 dragForce = Vec2(0, 0);

	if (particle.velocity.MagnitudeSquared() > 0) {
		Vec2 dragDirection = particle.velocity.UnitVector() * -1.0;

		float dragMagnitude = k * particle.velocity.MagnitudeSquared();
		
		// Fd = k(all real world constants) * ||v||^2 * -1.0
		dragForce = dragDirection * dragMagnitude;
	}
	return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Particle& particle, float k) {
	Vec2 frictionForce = Vec2(0, 0);

	Vec2 frictionDirection = particle.velocity.UnitVector() * -1.0;

	frictionForce = frictionDirection * k;
	return frictionForce;
}

Vec2 Force::GenerateGravitationalForce(const Particle& a, const Particle& b, float G, float minDistance, float maxDistance) {
	Vec2 d = (b.position - a.position);

	float distanceSquared = d.MagnitudeSquared();

	// Clamping to keep things spicy
	distanceSquared = std::clamp(distanceSquared, minDistance, maxDistance);

	Vec2 attractionDirection = d.UnitVector();
	float attractionMagnitude = G * (a.mass * b.mass) / distanceSquared;

	Vec2 attractionForce = attractionDirection * attractionMagnitude;

	return attractionForce;
}

Vec2 Force::GenerateSpringForce(const Particle& particle, Vec2 anchor, float restLength, float k) {
	// Using hooks law: F = -k * delta L

	Vec2 d = particle.position - anchor;

	float displacement = d.Magnitude() - restLength;
	
	Vec2 springDirection = d.UnitVector();
	float springMagnitude = -k * displacement;

	Vec2 springForce = springDirection * springMagnitude;
	return springForce;
}