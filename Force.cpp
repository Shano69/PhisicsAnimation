#define _USE_MATH_DEFINES
#include <cmath>
#include "Force.h"
#include "Body.h"

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
		 return glm::vec3(0.0f);
}