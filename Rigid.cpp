
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
/*
Vectors and Arrays
*/
glm::vec3 cpoint = glm::vec3(-6.0f, 0.0f, -6.0f);
glm::vec3 size = glm::vec3(12.0f, 12.0f, 12.0f);




int main()
{


	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));

	
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(20.0f, 5.0f, 20.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));
	Gravity g = Gravity();
	Drag drag = Drag();
	Wind wind = Wind();
	wind.setWind(glm::vec3(10.0f, 0.0f, .0f));
	//g.setGravity(glm::vec3(0.0f, 50.0f, 0.0f));

	// Set up a cubic rigid body .
	 RigidBody rb = RigidBody();
	 Mesh m = Mesh::Mesh(Mesh::CUBE);
	 rb.setMesh(m);
	 Shader rbShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");
	 rb.getMesh().setShader(rbShader);
	
		 // rigid body motion values
		 rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	 rb.setVel(glm::vec3(0.0f, 7.0f, 0.0f));
	 rb.setAngVel(glm::vec3(2.0f, 2.0f, 0.0f));
	 rb.setMass(1.0f);
		 // add forces to Rigid body
		 rb.addForce(&g);
	// new time	
	const float dt = 0.01f;
	float accumulator = 0.0f;
	GLfloat currentTime = (GLfloat)glfwGetTime();

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//New frame time
		GLfloat newTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		app.doMovement(dt);

		rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel()));
		
		 
		for (auto &ver:rb.getMesh().getVertices()) {
			auto& idk = rb.getMesh().getModel() * glm::vec4(ver.getCoord(), 1.0f);
			
			if (idk.y/idk.w  < 0.0f)
			{
				std::cout << "Collision";
			}
	}
		while (accumulator >= dt)
		{
			rb.getVel() = rb.getVel() + dt * rb.getAcc();
			rb.setPos(rb.getPos() + dt*(rb.getVel()));
			
			glm::vec3 dRot = rb.getAngVel() * dt;
			 if (glm::dot(dRot, dRot) > 0) 
			 {
				 rb.rotate(sqrt(glm::dot(dRot, dRot)), dRot);	
			 }


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
