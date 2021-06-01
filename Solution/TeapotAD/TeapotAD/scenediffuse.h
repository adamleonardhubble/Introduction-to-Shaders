#ifndef SCENEDIFFUSE_H
#define SCENEDIFFUSE_H

#include "gl_core_4_3.hpp"

#include <glfw3.h>
#include "scene.h"
#include "glslprogram.h"

#include "vboteapot.h"
#include "vboplane.h"

#include <glm.hpp>

using glm::mat4;


namespace imat2908
{

class SceneDiffuse : public Scene
{
private:
    GLSLProgram prog;

    int width, height;
 
	VBOTeapot *teapot;  //Teapot VBO
	VBOPlane *plane;  //Plane VBO

    mat4 model; //Model matrix
	mat4 teapotModel; //TeapotModel matrix
	mat4 lidModel; //LidModel matrix
	
	float angle = 0; //Angle of teapotModel

    void setMatrices(QuatCamera camera); //Set the camera matrices

    void compileAndLinkShader(); //Compile and link the shader

public:
    SceneDiffuse(); //Constructor

	float attenuationValue = 15.0f; //Value of attentuation (light)
	float specularValue = 30.0f; //Value of shininess

	vec3 worldLight; //WorldLight variable (vector 3)
	bool bMaxX = false; //WorldLight boolean condition

	void attenuationUp(); //Increase attenuation in the scene
	void attenuationDown(); //Decrease attentuation in the scene

	void specularUp(); //Increase specular properties of objects in the scene
	void specularDown(); //Decrease specular properties of objects in the scene

	void setLightParams(QuatCamera camera); //Setup the lighting

    void initScene(QuatCamera camera); //Initialise the scene

    void update(float t); //Update the scene

    void render(QuatCamera camera);	//Render the scene

    void resize(QuatCamera camera, int, int); //Resize
};

}

#endif // SCENEDIFFUSE_H
