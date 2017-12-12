#pragma once

#include "IntersectData.h"
#include "Colliders.h"
#include <algorithm>
#include <cmath>
#include <array>




	typedef struct SAT
	{
		const glm::vec3& getMin(OBB obb);
		const glm::vec3& getMax(OBB obb);
		Interval getInterval(OBB obb, const glm::vec3 axis);
		bool overlapOnAxis(OBB obb1, OBB obb2, const glm::vec3 axis);
	} SAT;



	enum class TYPE
	{
		SPHERE,
		OBB,
	};

	class ColliderTester
	{
	public:
		ColliderTester() { };
		ColliderTester(const TYPE& type) : m_type(type) {}

		inline void setType(TYPE type) { m_type = type; }
		inline const TYPE& getType() const { return m_type; }

		// OBBs with :
		IntersectData intersect(OBB obb1, OBB obb2);
		IntersectData intersect(OBB obb, Plane plane);

		// BoundingSpheres with :
		IntersectData intersect(BoundingSphere sphere1, BoundingSphere sphere2);
		IntersectData intersect(BoundingSphere sphere, OBB obb);
		IntersectData intersect(BoundingSphere sphere, Plane plane);
		IntersectData FindCollisionFeatures(OBB A, OBB B);
		static inline Plane getPlaneCollider() { return Plane(); }
		//private:
		glm::vec3 closestPoint(OBB obb, const glm::vec3& point);
		glm::vec3 closestPoint(BoundingSphere sphere, const glm::vec3& point);

		Interval getInterval(OBB obb, const glm::vec3 axis);

	private:
		TYPE m_type;
		std::vector<glm::vec3> getVertices( OBB obb); 
		std::vector<Line> getEdges( OBB obb); 
		std::vector<Plane> getPlanes( OBB obb);
		bool clipToPlane( Plane plane, const Line& line, glm::vec3* outPoint); 
		std::vector<glm::vec3> clipEdgesToOBB(const std::vector<Line>& edges, 	OBB obb);
		float penetrationDepth( OBB o1,  OBB o2, const glm::vec3& axis, bool* outShouldFlip);

	};
