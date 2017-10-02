#pragma once
// project includes
// !!!!!!!!!IMPORTANT!!!!!!!!!!!!!!
//DEFINE EITHER TaskTwo or TaskThree TO SEE DIFFERENT SCENES
#define TaskThree
#include "Particle.h"
#include "Application.h"
#include "Shader.h"

#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <array>

// Other Libs
#include "SOIL2/SOIL2.h"

//WINDBLOWER FORCE FIELD SIZE
glm::vec3 xmin = glm::vec3(-2.0f, 1.0f, -2.0f);
glm::vec3 xmax = glm::vec3(2.0f, 5.0f, 2.0f);


//vectors
glm::vec3 grav = glm::vec3(.0f, -9.8f, .0f);
glm::vec3 force = glm::vec3(.0f, .0f, .0f);
glm::vec3 cpoint = glm::vec3(-6.0f, 0.0f, -6.0f);
glm::vec3 size = glm::vec3(12.0f, 12.0f, 12.0f);
glm::vec3 drag = glm::vec3(.0f, .0f, .0f);
//glm::vec3 pos = glm::vec3(.0f, .0f, .0f);
glm::vec3 wind = glm::vec3(5.0f, 1.0f, 5.0f);

float friction;
// main function
void Task2(Particle &first, Particle &second, Particle &third, float &accumulator, const float &dt);
void Task3(std::array<Particle, 3> &particles, float &accumulator, const float &dt, const glm::vec3 &xmin, const glm::vec3 &xmax, glm::vec3 &wind);

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

	// create particle
	Particle particle;
#ifdef TaskTwo
	particle.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	particle.scale(glm::vec3(3.f, 3.f, 3.f));
	particle.setVel(glm::vec3(.0f, .0f, 0.0f));
#endif
#ifdef TaskThree
	particle.translate(glm::vec3(-6.0f, 2.5f, 0.0f));
	particle.scale(glm::vec3(3.f, 3.f, 3.f));
	particle.setVel(glm::vec3(2.0f, 2.0f, 0.0f));
#endif // TaksThree
	particle.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	// create explicit particle
	Particle ex;
#ifdef TaskTwo
	ex.translate(glm::vec3(-1.0f, 2.5f, .0f));
	ex.scale(glm::vec3(3.f, 3.f, 3.f));
	ex.setVel(glm::vec3(0.0f, 2.0f, 0.0f));
#endif


#ifdef TaskThree
	ex.translate(glm::vec3(6.0f, 2.5f, .0f));
	ex.scale(glm::vec3(3.f, 3.f, 3.f));
	ex.setVel(glm::vec3(-2.0f,2.0f, 0.0f));
#endif
	ex.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
	
	//create semi implicit particle
	Particle sim;
#ifdef TaskTwo
	sim.translate(glm::vec3(1.0f, 2.5f, 0.0f));
	sim.scale(glm::vec3(3.f, 3.f, 3.f));
	sim.setVel(glm::vec3(0.0f, 1.0f, .0f));
#endif
#ifdef TaskThree
	sim.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	sim.scale(glm::vec3(3.f, 3.f, 3.f));
	sim.setVel(glm::vec3(0.0f, 1.0f, 3.0f));
#endif
	sim.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	std::array<Particle, 3> particles { particle, ex, sim };

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

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(dt);

		#ifdef TaskThree
			Task3(particles, accumulator, dt, xmin, xmax, wind);

		while (accumulator >= dt)
		{
			//semi implicit
			particles[1].getVel() = particles[1].getVel() + dt*particles[1].getAcc();
			particles[1].setPos(particles[1].getPos() + dt*(particles[1].getVel()));

			particles[2].getVel() = particles[2].getVel() + dt*particles[2].getAcc();
			particles[2].setPos(particles[2].getPos() + dt*(particles[2].getVel()));

			particles[0].getVel() = particles[0].getVel() + dt*particles[0].getAcc();
			particles[0].setPos(particles[0].getPos() + dt*(particles[0].getVel()));

			accumulator -= dt;
	}
#endif
#ifdef TaskTwo
		Task2(particle, ex, sim, accumulator, dt);
#endif
		for(auto &part:particles)
		//COLLISION
		for (int i = 0; i < 3; i++)
		{

			if (part.getTranslate()[3][i] < cpoint[i] + 0.1f || part.getTranslate()[3][i] > size[i] + cpoint[i] - 0.1f)
			{
				part.getVel()[i] *= -1.f;
				//drag[i] *= -.9f;
			}
			
			//secod two if-s check if the particle is stationary on a surface 
			//and make sure it does not float off
			if (part.getPos()[i] < cpoint[i] + 0.1f)
			{
				part.getPos()[i] = cpoint[i] + 0.1f;

			}

			if (part.getPos()[i] > size[i] + cpoint[i] - 0.1f)
			{
				part.getPos()[i] = size[i] + cpoint[i] - 0.1f;


			}

		}
		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
#ifdef TaskTwo
		app.draw(ex.getMesh());
		app.draw(sim.getMesh());
		app.draw(particle.getMesh());
#endif
		// draw groud plane
		app.draw(plane);
#ifdef TaskThree

		for (auto  &part : particles)
		{
			app.draw(part.getMesh());
		}
#endif // TaskThree
		app.display();
		}
	
		
	app.terminate();

		return EXIT_SUCCESS;
	}


void Task3(std::array<Particle, 3> &particles, float &accumulator, const float &dt, const glm::vec3 &xmin, const glm::vec3 &xmax, glm::vec3 &wind)
{
	for (auto &part : particles)
	{
		//------------basic movement for explicit particle---------------------------
		//force = mass * grav;
		force = part.getMass() * grav; 
		 //acceleration = force / mass
		part.setAcc(force / part.getMass());

		//FIRST IFS CHECK FOR THE TOP AND BOTTOM PART OF THE FORCE FIELD 
		//SECOND TWO CHECK FOR SIDE COLISIONS BY THE X AXIS
		//THIRD CHECK FOR COLLISIONS ON THE Z AXIS
			if(part.getPos()[1]>xmin[1] && part.getPos()[1]<xmax[1] &&
				part.getPos()[0]>xmin[0]-((part.getPos()[1]-xmin[1])/2)&&
				part.getPos()[0]<xmax[0] + ((part.getPos()[1] - xmin[1]) / 2) &&
				part.getPos()[2]>xmin[2] - ((part.getPos()[1] - xmin[1]) / 2) &&
				part.getPos()[2]<xmax[2] + ((part.getPos()[1] - xmin[1]) / 2))
			{
				// BASIC FORCE ADDITION BY THE Y AXIS GROWING THE CLOSER WE GET TO THE MINIMUM Y OF THE FORCE FIELD
				// CHECKS TO SEE ON WHICH SIDE OF THE X AND Z AXIS AND APPLYING A FORCE IN THE RIGHT DIRECTION
				part.getVel()[1] = part.getVel()[1] +  wind[1] - (wind[1] * (part.getPos()[1] - xmin[1])* .2f );
				if(part.getPos()[0]>(xmax[0]+xmin[0])/2)
				{
					part.getVel()[0] += (wind[0] - (part.getPos()[0] - (xmax[0] + xmin[0]) / 2))/50;
				}
				if (part.getPos()[0]<(xmax[0] + xmin[0]) / 2)
				{
					
					part.getVel()[0] += -(wind[0] - ((xmax[0] + xmin[0]) / 2) - part.getPos()[0] )/50;
				}
				if (part.getPos()[2]>(xmax[2] + xmin[2]) / 2)
				{
					part.getVel()[2] += (wind[2] - (part.getPos()[2] - (xmax[2] + xmin[2]) / 2)) / 50;
				}
				if (part.getPos()[2]<(xmax[2] + xmin[2]) / 2)
				{

					part.getVel()[2] += -(wind[2] - ((xmax[2] + xmin[2]) / 2) - part.getPos()[2]) / 50;
				}
				
			}

	}
	//Movement
	

}
void Task2(Particle &first, Particle &second, Particle &third, float &accumulator, const float &dt)
{
	//------------basic movement for explicit particle---------------------------
	//force = mass * grav;
	force = second.getMass() * grav; 
	//acceleration = force / mass
	second.setAcc(force / second.getMass());

	//------------basic movement for semi implicit particle---------------------------
	//force = mass * grav;
	force = third.getMass() * grav; 
	 //acceleration = force / mass
	third.setAcc(force / third.getMass());

	//Movement

	while (accumulator >= dt)
	{
		//explicit - GAINS ENERGY AT REGARDLESS OF FRAME STEP
		glm::vec3 dex = glm::vec3(dt*second.getVel());
		second.setPos(second.getPos() + dt*(second.getVel()));
		second.getVel() = second.getVel() + dt*second.getAcc();
		//semi implicit -CONSERVES ENERGY BETTER STILL GAINS ENERGY ON A VERY SMALL FRAMESTEP
		glm::vec3 dsim = glm::vec3(dt*third.getVel());
		third.getVel() = third.getVel() + dt*third.getAcc();
		third.setPos(third.getPos() + dt*(third.getVel()));

		accumulator -= dt;
	}

	//Collision 
	//for and first if check for collision with any of the surfaces and change the direction

	for (int i = 0; i < 3; i++)
	{

		if (second.getTranslate()[3][i] < cpoint[i] + 0.1f || second.getTranslate()[3][i] > size[i] + cpoint[i] - 0.1f)
		{
			second.getVel()[i] *= -1.f;
		}
		if (third.getTranslate()[3][i] < cpoint[i] + 0.1f || third.getTranslate()[3][i] > size[i] + cpoint[i] - 0.1f)
		{
			third.getVel()[i] *= -1.f;
		}
		//secod two if-s check if the particle is stationary on a surface 
		//and make sure it does not float off
		if (second.getPos()[i] < cpoint[i] + 0.1f)
		{
			second.getPos()[i] = cpoint[i] + 0.1f;

		}

		if (second.getPos()[i] > size[i] + cpoint[i] - 0.1f)
		{
			second.getPos()[i] = size[i] + cpoint[i] - 0.1f;


		}

		if (third.getPos()[i] < cpoint[i] + 0.1f)
		{
			third.getPos()[i] = cpoint[i] + 0.1f;

		}

		if (third.getPos()[i] > size[i] + cpoint[i] - 0.1f)
		{
			third.getPos()[i] = size[i] + cpoint[i] - 0.1f;


		}
	}


	
}