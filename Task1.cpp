#if 0
<<<<<<< HEAD

=======
>>>>>>> 63780dcbad1d58160779084c5011a25528dd0f7c
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
/*
Vectors and Arrays
*/
glm::vec3 cpoint = glm::vec3(-6.0f, 0.0f, -6.0f);
glm::vec3 size = glm::vec3(12.0f, 12.0f, 12.0f);


void addCollision(Particle &p)
{
	for (int i = 0; i < 3; i++)
	{

		if (p.getTranslate()[3][i] < cpoint[i] + 0.1f || p.getTranslate()[3][i] > size[i] + cpoint[i] - 0.1f)
		{
			p.getVel()[i] *= -1.f;
		}

		if (p.getPos()[i] < cpoint[i] + 0.1f)
		{
			p.getPos()[i] = cpoint[i] + 0.1f;

		}

		if (p.getPos()[i] > size[i] + cpoint[i] - 0.1f)
		{
			p.getPos()[i] = size[i] + cpoint[i] - 0.1f;


		}
	}
}
void createParticles(std::array<Particle, 5>&p, Force *g, Force *drag)
{
	float gap = 1.0f;
	for (int i = 0; i<5; i++)
	{
		p[i].translate(glm::vec3(1.0f + gap, 10.0f, 0.0f));
		p[i].scale(glm::vec3(3.f, 3.f, 3.f));
		p[i].setVel(glm::vec3(0.0f, 1.0f, .0f));
		p[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
		gap++;
		if (i > 0) {
			p[i].addForce(g);
			p[i].addForce(drag);
		}
	}
}

void setAcceleration(std::array<Particle, 5>&p)
{
	for (int i = 1; i < p.size(); i++)
	{
		p[i].setAcc(p[i].applyForces(p[i].getPos(), p[i].getVel()));
	}

}
void setVelocitySI(std::array<Particle, 5>&p, const float time)
{
	for (int i = 1; i<5; i++)
	{
		p[i].getVel() = p[i].getVel() + time*p[i].getAcc();
		p[i].setPos(p[i].getPos() + time*(p[i].getVel()));
	}
}
void renderParticles(std::array<Particle, 5>&p, Application app)
{
	for (int i = 0; i < 5; i++)
	{
		app.draw(p[i].getMesh());
	}
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
	plane.scale(glm::vec3(20.0f, 5.0f, 20.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));
	Gravity g = Gravity();
	Drag drag = Drag();
	/*
	Hooks pointing downard
	*/
	std::array<Hook, 4> hooks;
<<<<<<< HEAD
	float ks = 8.0f;
	float kd = 0.0f;
	for (auto &hook : hooks)
	{
		hook = Hook(ks, kd, 0.05f);
=======
	float ks = 10.0f;
	float kd = 5.0f;
	for (auto &hook : hooks)
	{
		hook = Hook(ks, kd, 0.5f);
>>>>>>> 63780dcbad1d58160779084c5011a25528dd0f7c

	}

	/*
	Hooks pointing upwards
	*/
	std::array<Hook, 4> bhooks;

	for (auto &hook : bhooks)
	{
		hook = Hook(ks, kd, 0.5f);
		ks *= 0.7f;
	}

	std::array<Particle, 5> particles;
	createParticles(particles, &g, &drag);
	/*
	Adds particles to the hook forces
	*/
	for (int i = 0; i<hooks.size(); i++)
	{
		hooks[i].setBodies(&particles[i], &particles[i + 1]);

	}
	for (int i = bhooks.size() - 1; i >= 0; i--)
	{
		bhooks[i].setBodies(&particles[i + 1], &particles[i]);

	}

	/*
	Add forces to 
	*/
	particles[1].addForce(&hooks[0]);
	particles[2].addForce(&hooks[1]);
	particles[3].addForce(&hooks[2]);
	particles[4].addForce(&hooks[3]);
	particles[1].addForce(&bhooks[1]);
	particles[2].addForce(&bhooks[2]);
	particles[3].addForce(&bhooks[3]);

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


<<<<<<< HEAD
		//addCollision(particles[1]);
		//addCollision(particles[2]);
		//addCollision(particles[3]);
		//addCollision(particles[4]);
=======
		addCollision(particles[1]);
		addCollision(particles[2]);
		addCollision(particles[3]);
		addCollision(particles[4]);
>>>>>>> 63780dcbad1d58160779084c5011a25528dd0f7c

		setAcceleration(particles);

		while (accumulator >= dt)
		{

			setVelocitySI(particles, dt);
			accumulator -= dt;

		}

		// clear buffer
		app.clear();

		app.draw(plane);
		renderParticles(particles, app);
		app.display();
	}


	app.terminate();

	return EXIT_SUCCESS;
}
<<<<<<< HEAD
#endif
=======

#endif
>>>>>>> 63780dcbad1d58160779084c5011a25528dd0f7c
