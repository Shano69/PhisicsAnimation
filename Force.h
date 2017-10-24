#pragma once
#include<glm/glm.hpp>
#include "glm/ext.hpp"
#include <iostream>

class Body;

class Force
{	
public:
	Force() {}
	~Force() {}

	virtual glm::vec3 Force::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel);
};

class Gravity : public Force {
		public:
		 // constructors
				Gravity() {}
			 Gravity(const glm::vec3 & gravity) { m_gravity = gravity; }
			
				 // get and set methods
			glm::vec3 getGravity() const { return m_gravity; }
			 void setGravity(glm::vec3 gravity) { m_gravity = gravity; }
			
				 // physics
			glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;
			
		private:
			 glm::vec3 m_gravity = glm::vec3(0.0f, -9.8f, 0.0f);
					
};

 /*
    ** DRAG CLASS
    */
class Drag : public Force {
public:
	Drag() {}
	// physics
	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;
};

/*
** HOOK CLASS
*/
class Hook : public Force {
public:
	Hook() {}
	Hook(float ks, float kd, float rest)
	{
		m_ks = ks;
		m_kd = kd;
		m_rest = rest;                        
	}

	inline void setBodies(Body *body1, Body *body2) { m_b1 = body1; m_b2 = body2; }

	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;

private:
	float m_ks;
	float m_kd;
	float m_rest;

	Body* m_b1;
	Body* m_b2;
	// physics
	
};

class Particle;

class Aero : public Force {
public:
	Aero() {}
	Aero(float ro, float cd )
	{
		w_ro = ro;		
		w_cd = cd;

	}
	inline void setParticles(Particle *p1, Particle *p2, Particle *p3) { m_p1 = p1; m_p2 = p2; m_p3 = p3; }
	inline void setWindVec(glm::vec3 windVec) { w_windVec = windVec; }
	// physics
	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;

private:
	float w_ro;
	float w_cd;
	Particle* m_p1;
	Particle* m_p2;
	Particle* m_p3;
	glm::vec3 w_windVec;


};

class Wind : public Force {
public:
	// constructors
	Wind() {}
	Wind(const glm::vec3 & gravity) { m_Wind = gravity; }

	// get and set methods
	glm::vec3 getWind() const { return m_Wind; }
	void setWind(glm::vec3 gravity) { m_Wind = gravity; }

	// physics
	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;

private:
	glm::vec3 m_Wind = glm::vec3(5.0f, 0.0f, 0.0f);

};