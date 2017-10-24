#if 0 

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
glm::vec3 cpoint = glm::vec3(-10.0f, 0.0f, -10.0f);
glm::vec3 size = glm::vec3(20.0f, 20.0f, 10.0f);

void addHooksLeftRight(Hook lh[10][9], Particle p[10][10])
{
	for (int i = 0; i<10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			lh[i][j].setBodies(&p[i][j], &p[i][j + 1]);
		}
	}

	for (int i = 0; i<10; i++)
	{
		for (int j = 1; j < 10; j++)
		{
			p[i][j].addForce(&lh[i][j - 1]);
		}
	}
}
void addHooksRightLeft(Hook rh[10][9], Particle p[10][10])
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 8; j >= 0; j--)
		{
			rh[i][j].setBodies(&p[i][j + 1], &p[i][j]);
		}
	}

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			p[i][j].addForce(&rh[ i][j]);
		}
	}
}
void addHooksDownUp(Hook dh[9][10], Particle p[10][10])
{
	for (int j = 0; j < 10; j++)
	{
		for (int i = 8; i >= 0; i--)
		{
			dh[i][j].setBodies(&p[i + 1][j], &p[i][j]);
		}
	}

	for (int j = 0; j<10; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			p[i][j].addForce(&dh[i][j]);
		}
	}
}
void addHooksUpDown(Hook h[9][10], Particle p[10][10])
{
	for (int j = 0; j<10; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			h[i][j].setBodies(&p[i][j], &p[i + 1][j]);
		}
	}

	for (int j = 0; j<10; j++)
	{
		for (int i = 1; i < 10; i++)
		{
			p[i][j].addForce(&h[i - 1][j]);
		}
	}
}
void addCollision(Particle &p)
{
	for (int i = 0; i < 3; i++)
	{

		if (p.getTranslate()[3][i] < cpoint[i] + 0.1f || p.getTranslate()[3][i] > size[i] + cpoint[i] - 0.1f)
		{
			p.getVel()[i] *= -0.50f;
		}

		if (p.getPos()[i] < cpoint[i] + 0.1f)
		{
			p.getPos()[i] = cpoint[i] + 0.1f;
			p.getVel() *= 0.5f;
		}

		if (p.getPos()[i] > size[i] - 0.1f)
		{
			p.getPos()[i] = size[i] + cpoint[i] - 0.1f;
			p.getVel() *= 0.5f;
		}
	}
}

void createParticles(Particle p[10][10], Force *g, Force *drag, Force *wind)
{
	float gapx = 1.0f;
	float gapz = 1.0f;
	for (int i = 0; i<10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			p[i][j].translate(glm::vec3(-3.0f + gapx, 10.0f, 0.0f + gapz));
			p[i][j].scale(glm::vec3(3.f, 3.f, 3.f));
			p[i][j].setVel(glm::vec3(0.f, 1.0f, .0f));
			p[i][j].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
			p[i][j].setMass(1.0f);
			if ( j > 0  ) {
				p[i][j].addForce(g);
				p[i][j].addForce(drag);
				p[i][j].addForce(wind);
			}
			
			gapx++;
		}
		gapz--;
		gapx = 1.0f;
	}
}
void setAcceleration(Particle p[10][10])
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if ( j > 0  )
			{
				p[i][j].setAcc(p[i][j].applyForces(p[i][j].getPos(), p[i][j].getVel()));
			}
		}
	}

}
void setVelocitySI(Particle p[10][10], const float time)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (j > 0 )
			{
				p[i][j].getVel() = p[i][j].getVel() + time*p[i][j].getAcc();
				p[i][j].setPos(p[i][j].getPos() + time*(p[i][j].getVel()));
			}
		}
	}
}
void renderParticles(Particle p[10][10], Application app)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j<10; j++)
			app.draw(p[i][j].getMesh());
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
	Aero aero = Aero();
	Wind wind = Wind();
	wind.setWind(glm::vec3(0.0f, 0.0f, .0f));
	/*
	Hooks pointing downard
	*/
	float ks = 10.0f;
	float kd = 9.0f;

	std::array<Hook, 9> hooks;
	Hook h[9][10];
	
	for (int i = 0; i<9; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			h[i][j] = Hook(ks, kd, 1.f);
		}
	}

	Hook dh[9][10];
	for (int i = 0; i<9; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			dh[i][j] = Hook(ks, kd, 1.f);
		}
	}
	Hook lh[10][9];
	for (int i = 0; i<10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			lh[i][j] = Hook(ks, kd, 0.005f);
		}
	}
	Hook rh[10][9];
	for (int i = 0; i<10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			rh[i][j] = Hook(ks, kd, 0.005f);
		}
	}
	



	Particle p[10][10];
	std::array<Particle, 10> particles;
	createParticles(p, &g, &drag, &wind);
	
	aero = Aero(1.25f, .8f);
	aero.setWindVec(wind.getWind());
	for(int i =0; i<8;i++)
	{
		for (int j = 0; j < 8; j++)
		{
			aero.setParticles(&p[i][j], &p[i+1][j], &p[i][j+1] );
			p[i][j].addForce(&aero);
			p[i+1][j].addForce(&aero);
			p[i][j+1].addForce(&aero);
			aero.setParticles(&p[i+1][j+1], &p[i + 1][j], &p[i][j + 1]);
			p[i+1][j+1].addForce(&aero);
			p[i + 1][j].addForce(&aero);
			p[i][j + 1].addForce(&aero);

		}
	}
	
	
	addHooksUpDown(h, p);
	addHooksDownUp(dh, p);
	addHooksLeftRight(lh, p);
	addHooksRightLeft(rh, p);
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
		frameTime *= 2.0f;
		currentTime = newTime;
		accumulator += frameTime;

		for (int i = 0; i<10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				addCollision(p[i][j]);
			}
		}

		app.doMovement(dt);


		setAcceleration(p);

		while (accumulator >= dt)
		{

			setVelocitySI(p, dt);
			accumulator -= dt;
			
	}

		// clear buffer
		app.clear();

		app.draw(plane);
		renderParticles(p, app);
		app.display();
	}


	app.terminate();

	return EXIT_SUCCESS;
}
#endif