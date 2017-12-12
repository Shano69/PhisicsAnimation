#include "Particle.h"
#include "Application.h"
#include "Shader.h"
#include "RigidBody.h"
#include <random>
#include <set>

// Std. Includes
#include <string>
#include <time.h>
#include <array>
#include <cmath>
// Other Libs
#include "SOIL2/SOIL2.h"
#include "Rigid.h"

/*
Vectors and Arrays
*/
glm::vec3 cpoint = glm::vec3(-6.0f, 0.0f, -6.0f);
glm::vec3 size = glm::vec3(12.0f, 12.0f, 12.0f);

bool isImpulseApplied = false;
bool canRespondStatic = false;
bool canRespondDynamic = false;

void respondToPlane(RigidBody& rb1, IntersectData data);
std::set<Vertex> collidingVerts(float yCoordinate, RigidBody &rb);
void collisionResponce(RigidBody &rb, Mesh plane, std::set<Vertex>collidingVertices);
void handleStaticCollision(RigidBody* rb, Vertex collisionPoint, float e, bool applyFriction = false);
std::vector<Vertex> detectCollisionWithPlane(RigidBody* rb, glm::vec3 plane);
Vertex calcCollisionPoint(std::vector<Vertex> collidingVertices, RigidBody* rb);
void applyImpulse(RigidBody* rb, glm::vec3& J, glm::vec3& appPoint, double duration = 0);

void respond(RigidBody& rb1, RigidBody& rb2, IntersectData data);

void handleCollisions(const std::vector<RigidBody*>& rigidBodies, Mesh plane)
{

	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		auto& rb = rigidBodies[i];

		auto collisionData = rb->canCollideStatic();
		canRespondStatic = collisionData.getHasIntersection();
		if (canRespondStatic)
		{
			
			std::set<Vertex> collidingVertices = collidingVerts(plane.getPos().y, *rb);
			bool collisionOccurs = collidingVertices.size() > 0;
			if (collisionOccurs)
			{
				collisionResponce(*rb, plane, collidingVertices);
			}
		}
	}

	// Do dynamic rigid body vs rigid body collisions
	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		auto& rb1 = rigidBodies[i];
		for (size_t j = i + 1; j < rigidBodies.size(); j++)
		{
			auto& rb2 = rigidBodies[j];

			// perform intersection check
			auto collisionData = rb1->canCollideDynamic(rb2);
			canRespondDynamic = collisionData.getHasIntersection();
			if (canRespondDynamic)
			{
				IntersectData data = rb1->canCollideDynamic(rb2);
				respond(*rb1,*rb2,data);
			
			}
		}
	}
}


void respond(RigidBody& rb1, RigidBody& rb2, IntersectData data)
{
	float invMass1 = 1.0f /rb1.getMass();
	float invMass2 = 1.0f / rb2.getMass();
	float invMassSum = invMass1 + invMass2;
	if (invMassSum == 0.0f) {
		return; // Both objects have infinate mass!
	}

	if (glm::length(rb1.getVel()) + glm::length(rb1.getAngVel()) < 0.2f)
	{
		rb1.getVel() = glm::vec3(0.0f);
		rb1.setAngVel(glm::vec3(0.0f));
		rb1.getAcc() = glm::vec3(0.0f);
		
	}


	if (glm::length(rb2.getVel()) + glm::length(rb2.getAngVel()) < 0.2f)
	{
		rb2.getVel() = glm::vec3(0.0f);
		rb2.setAngVel(glm::vec3(0.0f));
		rb2.getAcc() = glm::vec3(0.0f);
		

	}
	
	//local points of contact

		glm::vec3 sumPoints;
		int count = 0;
		for (int i = 0; i < data.getContacts().size(); i++)
		{
			sumPoints += data.getContacts()[i];
			count++;
		}
		sumPoints = sumPoints / count;
		glm::vec3 r1 = sumPoints -rb1.getPos();
		glm::vec3 r2 = sumPoints - rb2.getPos() ;

		glm::mat4 i1 = rb1.getInvInertia();
		glm::mat4 i2 = rb2.getInvInertia();

		// Relative velocity
		glm::vec3 relativeVel = (rb2.getVel() + glm::cross(rb2.getAngVel(), r2)) - (rb1.getVel() + glm::cross(rb1.getAngVel(), r1));
		// Relative collision normal
		glm::vec3 relativeNorm = data.getNormal();
		relativeNorm = glm::normalize(relativeNorm);

		

		if (glm::dot(relativeVel, relativeNorm) > 0.0f) {
			return;
		}

		float e = 0.01f;
		float numerator = (-(1.0f + e)* glm::dot(relativeVel, relativeNorm));
		float d1 = invMassSum;
		glm::vec3 d2 = glm::cross((glm::vec3(glm::vec4(glm::cross(r1, relativeNorm),1.0f) * i1)), r1);
		glm::vec3 d3 = glm::cross((glm::vec3(glm::vec4(glm::cross(r2, relativeNorm), 1.0f) * i2)), r2);
		float denominator = d1 + glm::dot(relativeNorm, d2 + d3);
		float j = (denominator == 0.0f) ? 0.0f :
			numerator / denominator;
		if (data.getContacts().size() > 0 && j != 0.0f) {
			j /= (float)data.getContacts().size();
		}

		glm::vec3 impulse = relativeNorm * j;

	/*	std::cout <<"R1"<< glm::to_string(r1) << std::endl;
		std::cout << "R2" << glm::to_string(r2) << std::endl;
		std::cout << "Sumpoints" << glm::to_string(sumPoints) << std::endl;
		std::cout <<"Rigidbody OBB"<< glm::to_string(rb1.getOrientedBoxCollider().getPosition()) << std::endl;
		std::cout << "Rigidbody " << glm::to_string(rb1.getPos()) << std::endl;
		for (int i = 0; i < data.getContacts().size(); i++)
		{
			std::cout << "Contacts" << glm::to_string(data.getContacts()[i]) << std::endl;
		}*/

		auto halfDepth = data.getDepth() / 2.0f;
			rb2.translate(halfDepth*relativeNorm);


		
/*
		rb1.setVel(glm::vec3(0.0f));
		rb1.setAngVel(glm::vec3(0.0f));
		rb1.setAcc(glm::vec3(0.0f));

		rb2.setVel(glm::vec3(0.0f));
		rb2.setAngVel(glm::vec3(0.0f));
		rb2.setAcc(glm::vec3(0.0f));*/




		if (glm::length(impulse) > 0.0f)
		{
			
			rb1.getVel() = rb1.getVel() - impulse * invMass1;
			rb2.getVel() = rb2.getVel() + impulse * invMass2;
			rb1.setAngVel(rb1.getAngVel() - (glm::vec3(glm::vec4(glm::cross(r1, impulse), 0.0f) * i1)));
			rb2.setAngVel(rb2.getAngVel() + (glm::vec3(glm::vec4(glm::cross(r2, impulse), 0.0f) * i2)));
		}

		glm::vec3 t = relativeVel - (relativeNorm* glm::dot(relativeVel, relativeNorm));

		if (CMP(glm::length2(t), 0.0f)) {
			return;
		}
		t = glm::normalize(t);

		numerator = -glm::dot(relativeVel, t);
		d1 = invMassSum;
		d2 = glm::cross(glm::vec3( glm::vec4(glm::cross(r1, t),1.0f)* i1), r1);
		d3 = glm::cross(glm::vec3(glm::vec4(glm::cross(r2, t), 1.0f)* i2), r2);
		denominator = d1 + glm::dot(t, d2 + d3);

		if (denominator == 0.0f) {
			return;
		}

		float jt = numerator / denominator;
		if (data.getContacts().size() > 0.0f &&jt != 0.0f) {
			jt /= (float)data.getContacts().size();
		}

		if (isgreaterequal(jt, 0.0f)) {
			return;
		}

		float friction = 0.3f;
		if (jt> j * friction) {
			jt = j * friction;
		}
		else if (jt< -j * friction) {
			jt = -j * friction;
		}

		glm::vec3 tangentImpuse = t * jt;
	/*	if (glm::length(tangentImpuse) > 0.0f)
		{
			rb1.getVel() = rb1.getVel() + tangentImpuse * invMass1;
			rb2.getVel() = rb2.getVel() - tangentImpuse * invMass2;
			rb1.setAngVel(rb1.getAngVel() + glm::vec3(glm::vec4(glm::cross(r1, tangentImpuse), 1.0f)* i1));
			rb2.setAngVel(rb2.getAngVel() - glm::vec3(glm::vec4(glm::cross(r2, tangentImpuse), 1.0f)* i2));
		}*/
		glm::vec3 vt = relativeVel - glm::dot(relativeVel, relativeNorm) * relativeNorm;
		float mu = 0.5f;
		glm::vec3 jFriction = -mu * glm::length(j) * glm::normalize(vt);

		if (glm::length(jFriction) > 0.0f) {
			rb1.setVel(rb1.getVel() - (jFriction / rb1.getMass()));
			rb1.setAngVel(rb1.getAngVel() - (rb1.getInvInertia() * glm::cross(r1, jFriction)));
			rb2.setVel(rb2.getVel() + (jFriction / rb2.getMass()));
			rb2.setAngVel(rb2.getAngVel() + (rb2.getInvInertia() * glm::cross(r2, jFriction)));
		}


}
std::set<Vertex> collidingVerts(float yCoordinate, RigidBody &rb)
{
	std::set<Vertex> collidingVertices;

	// Check if it collides with plane
	for (auto v : rb.getMesh().getVertices())
	{

		// check the y coord of: localCoord * sclae + worldCoord
		// note: with opengl you need to use mat * vec
		glm::vec3 wCoord = glm::mat3(rb.getMesh().getModel()) * v.getCoord() + rb.getPos();
		if (wCoord.y < yCoordinate)
		{
			collidingVertices.insert(wCoord);

		}
	}

	return collidingVertices;
}

void collisionResponce(RigidBody &rb, Mesh plane, std::set<Vertex>collidingVertices)
{
	//std::cout << "collides" << std::endl;
	Vertex lowestVertex;
	// translate up on y axis by the lowest vertex on the y axis
	for (auto v : collidingVertices)
	{

		lowestVertex = v;

	}

	for (auto v : collidingVertices)
	{
		if (v.getCoord().y < lowestVertex.getCoord().y)
		{
			lowestVertex = v;
		}
	}

	glm::vec3 displacement = glm::vec3(0.0f);
	displacement.y = glm::abs(lowestVertex.getCoord().y);
	rb.translate(displacement);
	if (glm::length(rb.getVel()) + glm::length(rb.getAngVel()) < 0.05f)
	{
		rb.getVel() = glm::vec3(0.0f);
		rb.setAngVel(glm::vec3(0.0f));
		rb.getAcc() = glm::vec3(0.0f);
		displacement.y = 0.0f;
	}


	glm::vec3 sum = glm::vec3(0.0f);
	for (auto v : collidingVertices)
	{
		sum += v.getCoord();
	}
	Vertex averageCollidingPoint = Vertex(sum / collidingVertices.size());

	//************************************************************************************************************************
	//Impulse
	glm::vec3 r = averageCollidingPoint.getCoord() - rb.getPos();
	glm::vec3 relVel = rb.getVel() + glm::cross(rb.getAngVel(), r);
	glm::vec3 n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
	float e = 0.2f;
	glm::vec3 j = (-(1 + e) * relVel * n) / (pow(rb.getMass(), -1) + n * glm::cross((rb.getInvInertia() * (glm::cross(r, n))), r));
	// apply j
	if (glm::length(j) > 0) {
		rb.setVel(rb.getVel() + (j / rb.getMass()));
		rb.setAngVel(rb.getAngVel() + (rb.getInvInertia() * glm::cross(r, j)));
	}
	// friction
	glm::vec3 vt = relVel - glm::dot(relVel, n) * n;
	float mu = 0.5f;
	glm::vec3 jFriction = -mu * glm::length(j) * glm::normalize(vt);

	if (glm::length(j) > 0) {
		rb.setVel(rb.getVel() + (jFriction / rb.getMass()));
		rb.setAngVel(rb.getAngVel() + (rb.getInvInertia() * glm::cross(r, jFriction)));
	}



}


int main()
{

	
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));


	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	//Mesh plane = Mesh::Mesh("resources/models/plane10.obj");
	plane.setShader(Shader("resources/shaders/physics.vert", "resources/shaders/transp.frag"));
	plane.scale(glm::vec3(20.0f, 1.0f, 20.0f));
	plane.translate(glm::vec3(0.0f, 0.0f, 0.0f));
	Gravity g = Gravity();
	Drag drag = Drag();
	Wind wind = Wind();
	wind.setWind(glm::vec3(1.0f, 0.0f, .0f));



	
	// BODY 2
	RigidBody rb2 = RigidBody();
	rb2.setMesh(Mesh::CUBE);
	rb2.getMesh().setShader(Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag"));
	rb2.setMass(2.0f);
	rb2.translate(glm::vec3(3.0f, 5.0f, 0.0f));
	rb2.setVel(glm::vec3(-4.0f, 0.0f, 0.0f));
	rb2.scale(glm::vec3( 2.0f, 0.25f, 1.0f));
	//rb2.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb2.setAngVel(glm::vec3(3.0f, 0.0f, 5.0f));
	rb2.addCollider(TYPE::OBB);
	// add forces to Rigid body
	rb2.addForce(&g);

		// BODY 1
	RigidBody rb = RigidBody();
	rb.setMesh(Mesh::CUBE);
	rb.getMesh().setShader(Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag"));
	rb.setMass(2.0f);
	rb.translate(glm::vec3(0.0f, 4.0f, 0.0f));
	rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.scale(glm::vec3(0.25f, 2.0f, 1.0f));
	//rb2.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.addCollider(TYPE::OBB);
	// add forces to Rigid body
	rb.addForce(&g);

	// BODY 3
	RigidBody rb3 = RigidBody();
	rb3.setMesh(Mesh::CUBE);
	rb3.getMesh().setShader(Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag"));
	rb3.setMass(1.0f);
	rb3.translate(glm::vec3(-2.0f, 2.0f, 0.0f));
	rb3.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb3.scale(glm::vec3(0.25f, 2.0f, 1.0f));
	//rb2.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb3.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb3.addCollider(TYPE::OBB);
	// add forces to Rigid body
	rb3.addForce(&g);

	// BODY 4
	RigidBody rb4 = RigidBody();
	rb4.setMesh(Mesh::CUBE);
	rb4.getMesh().setShader(Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag"));
	rb4.setMass(1.0f);
	rb4.translate(glm::vec3(-4.0f, 2.0f, 0.0f));
	rb4.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb4.scale(glm::vec3(0.25f, 2.0f, 1.0f));
	//rb2.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb4.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb4.addCollider(TYPE::OBB);
	// add forces to Rigid body
	rb4.addForce(&g);



	// new time	
	const float dt = 0.003f;
	float accumulator = 0.0f;
	GLfloat currentTime = (GLfloat)glfwGetTime();

	
	std::vector<RigidBody*> rbCollection = { &rb,&rb2,&rb3,&rb4 };
		

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{

		//New frame time
		GLfloat newTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = newTime - currentTime;

		//*******************************************************************************************************************
		frameTime *= 2;
		currentTime = newTime;
		accumulator += frameTime;

		app.doMovement(dt);



		while (accumulator >= dt)
		{
			bool responded = false;
			if (!responded) {
				handleCollisions(rbCollection,plane);
				responded = true;
			}
			for (auto rb : rbCollection)
			{
				
				
				// integration position
				rb->setAcc(rb->applyForces(rb->getPos(), rb->getVel()));
				rb->getVel() = rb->getVel() + dt * rb->getAcc();
				rb->setPos(rb->getPos() + dt*(rb->getVel()));

				// integration  rotation

				rb->setAcc(rb->applyForces(rb->getPos(), rb->getVel()));

				rb->setAngVel(rb->getAngVel() + dt * rb->getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(rb->getAngVel());
				glm::mat3 R = glm::mat3(rb->getRotate());
				R += dt * angVelSkew * R;
				R = glm::orthonormalize(R);
				rb->getMesh().setRotate(glm::mat4(R));
				accumulator -= dt;
			}

		
		}

		// clear buffer
		app.clear();
		app.draw(rb2.getMesh());
		app.draw(rb.getMesh());
		app.draw(rb3.getMesh());
		app.draw(rb4.getMesh());

		app.draw(plane);
		app.display();
	}


	app.terminate();

	return EXIT_SUCCESS;
}
