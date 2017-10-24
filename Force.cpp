#define _USE_MATH_DEFINES
#include <cmath>
#include "Force.h"
#include "Particle.h"

 glm::vec3 Force::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	 return glm::vec3(0.0f);
}

 /*
    ** GRAVITY
    */
	 glm::vec3 Gravity::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	 // complete . Should return the acceleration resulting from gravity
	
		 return getGravity();
}

 /*
   ** DRAG
    */
	 glm::vec3 Drag::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	 // complete . Should return the acceleration resulting from aerodynamic drag
		 GLfloat dotVel = glm::dot(vel, vel);
		 
		 return -glm::vec3 (0.5f*0.05f*1.25f*dotVel*vel/sqrt(dotVel));
}
	 /*
	 HOOKS LAW
	 */
	 glm::vec3 Hook::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
	 {
		
		 //Distance between two points
		 float l = glm::distance(m_b1->getPos(), m_b2->getPos());
		 //Computing unit vector
		 glm::vec3 e = (m_b2->getPos() - m_b1->getPos())/l;
		 //Compute the 1D velocities for each of the particles
		 float vel1D1 = glm::dot(e, m_b1->getVel());
		 float vel1D2 = glm::dot(e, m_b2->getVel());
		 //Computing the 1D forces and mapping them to 3D
		 float f = -m_ks*(m_rest - l) - m_kd*(vel1D1 - vel1D2);
		
			return -(f*e);

	 }

	 glm::vec3 Aero::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
	 {
		 //V = (r1 + r2 + r3 )/3
		 glm::vec3 vSurf = (m_p1->getVel() + m_p2->getVel() + m_p3->getVel()) / 3;
		 glm::vec3 aVel = vSurf - w_windVec;
		 //a0= 0.5* ||r2-r1 CROSS r3-r1||
		 float area0 = 0.5*glm::length(glm::cross((m_p2->getPos() - m_p1->getPos()), (m_p3->getPos() - m_p1->getPos())));
		 //n= (r2-r1 CROSS r3-r1)/||r2-r1 CROSS r3-r1||
		 //glm::vec3 n = (glm::cross((m_p2->getPos() - m_p1->getPos()), (m_p3->getPos() - m_p1->getPos())))/ glm::length(glm::cross((m_p2->getPos() - m_p1->getPos()), (m_p3->getPos() - m_p1->getPos())));
		 glm::vec3 n = glm::normalize(glm::cross((m_p2->getPos() - m_p1->getPos()), (m_p3->getPos() - m_p1->getPos())));
		 //a = a0 * (V DOT n) / ||V||
		 float area = (area0 * (glm::dot(aVel, n))) / glm::length(aVel);
		 //Faero = 0.5*ro*||v||*||v||*Cd*a*n
		 glm::vec3 fAero = 0.5*w_ro*glm::length(aVel)*glm::length(aVel)*w_cd*area*n;
		
		 return -fAero / 3;
		
	 }

	 glm::vec3 Wind::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
		 // complete . Should return the acceleration resulting from gravity

		 return getWind();
	 }