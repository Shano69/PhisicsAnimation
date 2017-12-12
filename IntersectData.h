#pragma once
#include<glm/glm.hpp>
#include "glm/ext.hpp"
#include <iostream>
#include <vector>

typedef struct Interval
{
	float min;
	float max;
} Interval;

class IntersectData
{
	public:
		IntersectData() :
			m_hasIntersection(false),
			m_distance(0.0f),
			m_normal(glm::vec3(0.0f,0.0f,1.0f)),
			m_depth(FLT_MAX),
			m_contacts(std::vector<glm::vec3>())
		{};

		IntersectData(const bool hasIntersection, const float distance) :
			m_hasIntersection(hasIntersection),
			m_distance(distance)
		{}

		IntersectData(const bool hasIntersection,glm::vec3 normal, const float depth, const std::vector<glm::vec3>& contacts) :
			m_hasIntersection(hasIntersection),
			m_normal(normal),
			m_depth(depth),
			m_contacts(contacts)
			
		{}
		

		inline bool getHasIntersection() { return m_hasIntersection; }
		inline void setHasIntersection(bool hasIntersection) { m_hasIntersection = hasIntersection; }
		inline float getDistance() { return m_distance; }
		inline float getDepth() { return m_depth; }
		inline void setDepth(float depth) { m_depth = depth; }
		inline std::vector<glm::vec3> getContacts() { return m_contacts; }
		inline void insertContact(std::vector<glm::vec3> contacts) { m_contacts = contacts; }
		inline void setNormal(glm::vec3 normal) { m_normal = normal; }
		inline glm::vec3 getNormal() { return m_normal; }

	private:
		bool m_hasIntersection;
		float m_distance;

		glm::vec3 m_normal;
		float m_depth;
		std::vector<glm::vec3> m_contacts;
};