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
	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) override;};