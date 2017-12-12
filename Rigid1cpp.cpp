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
	rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.getMesh().scale(glm::vec3(1.0f, 3.0f, 1.0f));
	rb.setMass(2.0f);




	// add forces to Rigid body
	//rb.addForce(&g);
	// new time	
	const float dt = 0.01f;
	float accumulator = 0.0f;
	GLfloat currentTime = (GLfloat)glfwGetTime();

	bool appliedImpulse = false;
	bool appliedImpulse2 = false;
	bool appliedImpulse3 = false;

	std::cout << glm::to_string(rb.getInvInertia()) << std::endl;


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


//*************************************************************************************************************************************

			// apply impulse
			glm::vec3 j(1.0, 0.0, 0.0);

			if (currentTime > 2.0f && !appliedImpulse) 
			{
				rb.getVel() += j / rb.getMass();

				glm::vec3 applicationPoint = rb.getPos() + glm::vec3(1.0f,0.0f, 0.0f);

				rb.setAngVel(rb.getAngVel() + rb.getInvInertia() * glm::cross(applicationPoint - rb.getPos(), j));

				appliedImpulse = true;
			}

			if (currentTime > 5.0f && !appliedImpulse2)
			{
				rb.getVel() += -j / rb.getMass();

				glm::vec3 applicationPoint = rb.getPos() + glm::vec3(1.0f, 0.5f, 0.0f);

				rb.setAngVel(rb.getAngVel() + rb.getInvInertia() * glm::cross(applicationPoint - rb.getPos(), -j));

				appliedImpulse2 = true;
			}

			if (currentTime > 8.0f && !appliedImpulse3)
			{
				rb.getVel() += -j*3 / rb.getMass();

				glm::vec3 applicationPoint = rb.getPos() + glm::vec3(1.0f,- 0.5f, 0.6f);

				rb.setAngVel(rb.getAngVel() + rb.getInvInertia() * glm::cross(applicationPoint - rb.getPos(), -j*3));

				appliedImpulse3 = true;
			}



//******************************************************************************************************************

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