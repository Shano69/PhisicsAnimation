#include "RigidBody.h"



RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}


void RigidBody::updateInvInertia()
{
	float w = getMesh().getScale()[0][0] * 2.0f;
	float d = getMesh().getScale()[2][2] * 2.0f;
	float h = getMesh().getScale()[1][1] * 2.0f;

	glm::mat3 inertia = glm::mat3(0.0f);
	inertia[0][0] = getMass() / 12.0f * (h * h + d * d);
	inertia[1][1] = getMass() / 12.0f * (w * w + d * d);
	inertia[2][2] = getMass() / 12.0f * (w * w + h * h);

	m_invInertia = inverse(inertia);
}

IntersectData RigidBody::canCollideStatic()
{
	
	 if (m_collider.getType() == TYPE::OBB)
	{
		return m_collider.intersect(getOrientedBoxCollider(), ColliderTester::getPlaneCollider());
	}
	 if (m_collider.getType() == TYPE::SPHERE)
	{
		return m_collider.intersect(getSphereCollider(), ColliderTester::getPlaneCollider());
	}
	else
	{
		std::cout << "Error: Some of these STATIC collisions are not implemented yet.\n";
		return IntersectData();
	}
}

IntersectData RigidBody::canCollideDynamic(RigidBody* other)
{
	
	 if (m_collider.getType() == TYPE::OBB && other->getCollider().getType() == TYPE::OBB)
	{
		 return m_collider.FindCollisionFeatures(getOrientedBoxCollider(), other->getOrientedBoxCollider());
	}
	else if (m_collider.getType() == TYPE::OBB && other->getCollider().getType() == TYPE::SPHERE)
	{
		return m_collider.intersect(other->getSphereCollider(), getOrientedBoxCollider());
	}
	else if (m_collider.getType() == TYPE::SPHERE && other->getCollider().getType() == TYPE::SPHERE)
	{
		return m_collider.intersect(getSphereCollider(), other->getSphereCollider());
	}
	else if (m_collider.getType() == TYPE::SPHERE && other->getCollider().getType() == TYPE::OBB)
	{
		return m_collider.intersect(getSphereCollider(), other->getOrientedBoxCollider());
	}
	else
	{
		std::cout << "Error: Some of these DYNAMIC collisions are not implemented yet.\n";
		return IntersectData();
	}
}


OBB RigidBody::getOrientedBoxCollider()
{
	auto halfEdgeLen =  getScaleVec();
	return OBB(getPos(), glm::mat3(getRotate()), halfEdgeLen);
}

BoundingSphere RigidBody::getSphereCollider()
{
	auto R = (getScaleVec().x + getScaleVec().y + getScaleVec().z) / 3.0f;
	return BoundingSphere(getPos(), R);
}
