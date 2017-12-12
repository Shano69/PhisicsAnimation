#if 0
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
	rb.translate(glm::vec3(0.0f, 10.0f, 0.0f));
	rb.setVel(glm::vec3(5.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.getMesh().scale(glm::vec3(1.0f, 3.0f, 1.0f));
	rb.setMass(2.0f);




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
				Vertex lowestVertex;
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

				rb.getAcc() = glm::vec3(0.0f);
				rb.getVel() = glm::vec3(0.0f);
				rb.setAngVel(glm::vec3(0.0f));

				glm::vec3 sum = glm::vec3(0.0f);
				for (auto v : collidingVertices)
				{
					sum += v.getCoord();
				}
				Vertex averageCollidingPoint = Vertex(sum / collidingVertices.size());
				
				if (!check)
				{
				for (auto v : collidingVertices)
				{
					
						std::cout << "Coordinate of colliding vertex ->" << glm::to_string(v.getCoord())<<std::endl;
					
				}
				std::cout << "Point of collision ->" << glm::to_string(averageCollidingPoint.getCoord()) << std::endl;

				check = true;
				}
			}
			


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
#endif