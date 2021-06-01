
#include "scenediffuse.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

using std::cerr;
using std::endl;

#include "defines.h"

using glm::vec3;

#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>

namespace imat2908
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	// Default constructor
	/////////////////////////////////////////////////////////////////////////////////////////////
	SceneDiffuse::SceneDiffuse()
	{

	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Initialise the scene
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::initScene(QuatCamera camera)
	{
		// Compile and link the shader  
		compileAndLinkShader();

		gl::Enable(gl::DEPTH_TEST); // Renders object in scene relative to culling
		gl::Enable(gl::MULTISAMPLE); // Multisample fragment shader, used to antialiase edges (smoothen)

		// Set up the lighting
		setLightParams(camera);

		// Create the plane to represent the ground
		plane = new VBOPlane(100, 100, 100, 100);

		// A matrix to move the teapot lid upwards
		glm::mat4 lid = glm::mat4(1.0);

		lid = glm::translate(vec3(0.0, 0.0, 1.0));

		// Create the teapot with translated lid
		teapot = new VBOTeapot(16, lid);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Update scene, begin animating
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::update(float t)
	{
		if (animating() == true) // If animating is true, do the following every frame passed
		{
			if (worldLight.x < 20.0f && bMaxX == false) // If worldLight hasn't reached its maximum positive 'X' axis position, and bMaxX equals false, do the following
			{
				worldLight.x += 0.25f; // Add and equal the worldLigth value to itself '+ 0.25', per passing frame

				if (worldLight.x >= 20.0f) // If worldLight's 'X' axis value is larger than of equal to '20', do the following
				{
					bMaxX = true; // Set bMaxX true
				}
			}
			if (bMaxX == true) // If bMaxX is equal to true, do the following
			{
				worldLight.x -= 0.25f; // Subtract and equal the worldLigth value to itself '- 0.25', per passing frame

				if (worldLight.x <= -20.f) // If worldLight's 'X' axis value is smaller than of equal to '-20', do the following
				{
					bMaxX = false; // Set bMaxX false
				}
			}

			angle += 0.01f; // Add and equal to the angle, for every frame passed

			teapotModel *= glm::rotate(teapotModel, angle, glm::vec3(0.0f, 0.1f, 0.0f)); // Multiply and equal the teapotModels rotation to the rotation given, per passing frame

			prog.setUniform("LightPosition", worldLight); // LightPosition uniform variable

			float radAngle = angle * 180 / 3.1415926535897932384626433; // radAngle defining radians to degrees conversipon of 'angle'
			
			if (radAngle >= 360.0f && angle >= 6.283f) // If the angle the teapot has rotated, is equal to or more than '360', do the following
			{
				radAngle -= 360.0f; // Reset ranAngle (-360 degrees)
				angle -= 6.283f; // Reset angle (-6.283 radians)
			}

			std::cout << "Specular level: [" << specularValue << "]" << "\t" << "Attenuation level: [" << attenuationValue << "]" << "\t   " << "Angle of Utah teapot: [" << radAngle << "]" << std::endl; // Output 'specular', 'attenuation' and 'angle of Utah teapot' values to console
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Update the attenuation value to increase, used by key process events (teapotAD.cpp)
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::attenuationUp()
	{
		attenuationValue += 1.0f; // Add and equal the attenuation value to itself '+ 1'
		prog.setUniform("ADS.attenuationValue", attenuationValue); // Set the 'attenuationValue' uniform variable, to the current attentuation
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Update the attenuation value to decrease, used by key process events (teapotAD.cpp)
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::attenuationDown()
	{
		attenuationValue -= 1.0f; // Subtract and equal the attenuation value to itself '- 1'
		prog.setUniform("ADS.attenuationValue", attenuationValue); // Set the 'attenuationValue' uniform variable, to the current attentuation
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Update the specular value to increase, used by key process events (teapotAD.cpp)
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::specularUp()
	{
		specularValue += 1.0f; // Add and equal the specular value to itself '+ 1'
		prog.setUniform("Mat.Shininess", specularValue); // Set the 'shininess' uniform variable, to the current specular value
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Update the specular value to decrease, used by key process events (teapotAD.cpp)
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::specularDown()
	{
		specularValue -= 1.0f; // Subtract and equal the specular value to itself '- 1'
		prog.setUniform("Mat.Shininess", specularValue); // Set the 'shininess' uniform variable, to the current specular value
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Set up the lighting variables in the shader
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::setLightParams(QuatCamera camera)
	{
		worldLight = vec3(0.0f, 15.0f, 10.0f); // Translate position of the worldLight variable

		prog.setUniform("ADS.Ld", 0.9f, 0.9f, 0.9f); // Diffuse light intensity, coloured
		prog.setUniform("ADS.La", 0.3f, 0.3f, 0.3f); // Ambient light intensity, coloured
		prog.setUniform("ADS.Ls", 1.0f, 1.0f, 1.0f); // Specular light intensity, coloured

		prog.setUniform("LightPosition", worldLight); // LightPosition uniform variable, instantiated
		prog.setUniform("ADS.attenuationValue", attenuationValue); // attenuationValue uniform variable, instantiated
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Render the scene
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::render(QuatCamera camera)
	{
		gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

		// First deal with the plane to represent the ground
		
		model = mat4(1.0f); // Initialise the model matrix for the plane

		// Set the matrices for the plane although it is only the model matrix that changes so could be made more efficient
		setMatrices(camera);

		// Set the plane's material properties in the shader and render
		prog.setUniform("Mat.Kd", 0.5f, 0.5f, 0.0f); // Diffuse material reflectivity, coloured
		prog.setUniform("Mat.Ka", 0.7f, 0.4f, 1.0f); // Ambient material reflectivity, coloured
		prog.setUniform("Mat.Ks", 0.1f, 0.1f, 0.1f); // Specular material reflectivity, coloured

		plane->render(); // What does it do?

		model = mat4(1.0f); // Now set up the teapot
		
		model = teapotModel; // Assign model to teapotModel (used in update method)
		teapotModel = glm::translate(vec3(0.0f, 1.0f, 0.0f)); // Translate teapotModel in 'Y' axis (translate model)

		setMatrices(camera);

		// Set the Teapot material properties in the shader and render
		prog.setUniform("Mat.Kd", 1.0f, 1.0f, 0.0f); // Diffuse material reflectivity, coloured
		prog.setUniform("Mat.Ka", 0.9f, 0.3f, 0.6f); // Ambient material reflectivity, coloured
		prog.setUniform("Mat.Ks", 0.5f, 0.5f, 0.5f); // Specular material reflectivity, coloured

		prog.setUniform("Mat.Shininess", specularValue); // Shininess factor

		prog.setUniform("CameraPosition", camera.position()); // Fetch and use camera position, to calculate light relative to world space

		teapot->render(); // Render teapot
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Send the MVP matrices to the GPU
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::setMatrices(QuatCamera camera)
	{
		mat4 mv = camera.view() * model;

		prog.setUniform("ModelViewMatrix", mv);
		prog.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
		prog.setUniform("MVP", camera.projection() * mv);

		mat3 normMat = glm::transpose(glm::inverse(mat3(model))); // What does this line do?

		prog.setUniform("M", model);
		prog.setUniform("V", camera.view());
		prog.setUniform("P", camera.projection());

		prog.setUniform("CameraPosition", camera.position()); // Fetch and use camera position, to calculate light relative to world space

	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// resize the viewport
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::resize(QuatCamera camera, int w, int h)
	{
		gl::Viewport(0, 0, w, h);

		width = w;
		height = h;

		camera.setAspectRatio((float)w / h);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Compile and link the shader
	/////////////////////////////////////////////////////////////////////////////////////////////
	void SceneDiffuse::compileAndLinkShader()
	{
		try
		{
			prog.compileShader("Shaders/phong.vert");
			prog.compileShader("Shaders/phong.frag");
			prog.link();
			prog.validate();
			prog.use();
		}
		catch (GLSLProgramException & e) {
			cerr << e.what() << endl;
			exit(EXIT_FAILURE);
		}
	}
}
