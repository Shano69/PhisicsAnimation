#pragma once
#include "Body.h"
#include "ColliderTester.h"

class RigidBody :
	public Body
{
public:
	RigidBody();
	~RigidBody();


	
	OBB getOrientedBoxCollider();
	BoundingSphere getSphereCollider();

	// set and get methods
	void setAngVel(const glm::vec3 omega) { m_angVel = omega; }
	void setAngAcc(const glm::vec3 alpha) { m_angAcc = alpha; }
	void setInvInertia(const glm::mat3 &invInertia) { m_invInertia = invInertia; }
	void setMass(float mass) { m_mass = mass; updateInvInertia(); }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInvInertia() { return m_invInertia; }
	void scale(glm::vec3 vect) { m_mesh.scale(vect); updateInvInertia(); }
	inline const glm::vec3& getScaleVec() const { return glm::vec3(getScale()[0].x, getScale()[1].y, getScale()[2].z); }


	void updateInvInertia();

	ColliderTester getCollider() { return m_collider; }
	void addCollider(TYPE type) { m_collider.setType(type); }

	// COLLISION DETECTION FUNCTION
	IntersectData canCollideStatic();
	IntersectData canCollideDynamic(RigidBody* other);

	// CREATE COLLISION SHAPES FROM THE OBJECT
private:
	
	float m_density;
	glm::mat3 m_invInertia; 
	glm::vec3 m_angVel; 
	glm::vec3 m_angAcc; 
	ColliderTester m_collider;


};