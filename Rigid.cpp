//#if 0
#include "Particle.h"
#include "Application.h"
#include "Shader.h"
#include "RigidBody.h"
#include <random>


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
bool check = false;
bool check2 = false;
bool check3 = true;
bool respond = false;
bool collide = false;

glm::vec3 lowestPoint = glm::vec3(0.0f);
glm::vec3 pointOfCollision = glm::vec3(0.0f);


void friction(glm::vec3 j, glm::vec3 n ,glm::vec3 vr,glm::vec3 r,RigidBody &rb)
{
	float jt = j.x*n.x + j.y*n.y;
	glm::vec3 vt = vr - (glm::dot(vr, n)*n);
	glm::vec3 jn=jt*vr;
	if (glm::length(jn) > 0.9*glm::length(j))
	{
		jn = 0.9*j;
	}
	
	
	glm::vec3 jf = jn * vt;

	rb.setVel(rb.getVel() - (jf / rb.getMass()));
	rb.setAngVel(rb.getAngVel() - (rb.getInvInertia() * glm::cross(r, jf)));

}


void collisionResponce(RigidBody &rb, Mesh plane,std::set<Vertex>collidingVertices)
{
	std::cout << "collides" << std::endl;
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
	float e = 0.7f;
	glm::vec3 j = (-(1 + e) * relVel * n) / (pow(rb.getMass(), -1) + n * glm::cross((rb.getInvInertia() * (glm::cross(r, n))), r));
	// apply j
	rb.setVel(rb.getVel() + (j / rb.getMass()));
	rb.setAngVel(rb.getAngVel() + (rb.getInvInertia() * glm::cross(r, j)));

	// friction
		glm::vec3 vt = relVel - glm::dot(relVel, n) * n;
		float mu = 0.06;
		glm::vec3 jFriction = -mu * glm::length(j) * glm::normalize(vt);

		if (glm::length(j) > 0) {
			rb.setVel(rb.getVel() + (jFriction / rb.getMass()));
			rb.setAngVel(rb.getAngVel() + (rb.getInvInertia() * glm::cross(r, jFriction)));
		}
		if (abs(rb.getAngVel().y) < 0.01)
		{
			rb.setAngVel(glm::vec3(rb.getAngVel().x, rb.getAngVel().y / 100, rb.getAngVel().z));
		}
		if (abs(rb.getAngVel().x) < 0.01)
		{
			rb.setAngVel(glm::vec3(rb.getAngVel().x/100, rb.getAngVel().y , rb.getAngVel().z));
		}
		if (abs(rb.getAngVel().z) < 0.01)
		{
			rb.setAngVel(glm::vec3(rb.getAngVel().x, rb.getAngVel().y , rb.getAngVel().z/100));
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





int main()
{


	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));

	
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(20.0f, 1.0f, 20.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));
	Gravity g = Gravity();
	Drag drag = Drag();
	Wind wind = Wind();
	wind.setWind(glm::vec3(1.0f, 0.0f, .0f));
	
	// Set up a cubic rigid body .
	 RigidBody rb = RigidBody();
	 Mesh m = Mesh::Mesh(Mesh::CUBE);
	 rb.setMesh(m);
	 Shader rbShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");
	 rb.getMesh().setShader(rbShader);
	 std::set<Vertex> pointsCollided;

	// rigid body motion values
	 rb.translate(glm::vec3(0.0f,10.0f, 0.0f));
	 rb.setVel(glm::vec3(5.0f, 0.0f, 0.0f));
	 rb.setAngVel(glm::vec3(0.1f,0.1f,0.1f));
	 rb.getMesh().scale(glm::vec3(1.0f, 3.0f, 1.0f));
	 rb.setMass(1.0f);
	 
	 
	

		 // add forces to Rigid body
		 rb.addForce(&g);
	// new time	
	const float dt = 0.01f;
	float accumulator = 0.0f;
	GLfloat currentTime = (GLfloat)glfwGetTime();

	glm::vec3 j = glm::vec3(1.0f, 0.0f, 0.0f);
	bool appliedImpulse = false;
	
	
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
			std::set<Vertex> collidingVertices = collidingVerts(plane.getPos().y, rb);
			bool collisionOccurs = collidingVertices.size() > 0;
			if (collisionOccurs)
			{
				collisionResponce(rb,plane,collidingVertices);
			}
			// apply impulse
			/*if (currentTime > 2.0f && !appliedImpulse) {
				rb.getVel() += j / rb.getMass();

				glm::vec3 applicationPoint = rb.getPos() + glm::vec3(1.0f,0.1f, 0.0f);

				rb.setAngVel(rb.getAngVel() + rb.getInvInertia() * glm::cross(applicationPoint - rb.getPos(), j));
				
				std::cout << glm::to_string(rb.getInvInertia()) << std::endl;

				appliedImpulse = true;
			}*/
			

			 // integration position
			 rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel()));
			 rb.getVel() = rb.getVel() + dt * rb.getAcc();
			 rb.setPos(rb.getPos() + dt*(rb.getVel()));
			
			 // integration  rotation

			 rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel()));

			 rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
			 glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
			 glm::mat3 R = glm::mat3(rb.getRotate());
			 R += dt * angVelSkew * R;
			 R = glm::orthonormalize(R);
			 rb.getMesh().setRotate(glm::mat4(R));
			 accumulator -= dt;
		}

		// clear buffer
		app.clear();
		app.draw(rb.getMesh());
		app.draw(plane);
		app.display();
	}


	app.terminate();

	return EXIT_SUCCESS;
}
//#endif