#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define M_PI 3.14159265f

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"


// Window dimensions
const float toRadians = M_PI / 180.0f; // Converts a number times the variable to radians

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

bool isPerspective = true;

Texture planeTexture;
Texture keyboardTexture;
Texture mousepadTexture;
Texture keycapTexture;
Texture micstandTexture;
Texture micTexture;

Light mainLight;

Material shinyMaterial;
Material dullMaterial;

GLfloat deltaTime = 0.0f; // change in time
GLfloat lastTime = 0.0f;

// Vertex Shader Program
static const char* vShader = "Shaders/default.vert";
/* Fragment Shader Source Code*/
static const char* fShader = "Shaders/default.frag";

/* This function computes the average normals for a mesh by calculating face normals for triangles
*  and then normalizing them to get smoother normals for each vertex
*/
void calcAverageNorms(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vertLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i+=3) // increments 3 at a time - Checks each line of 3 indices at a time
	{
		unsigned int in0 = indices[i]     * vertLength;
		unsigned int in1 = indices[i + 1] * vertLength;
		unsigned int in2 = indices[i + 2] * vertLength;

		glm::vec3 vector1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 vector2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(vector1, vector2);
		normal = glm::normalize(normal);

		in0 += normalOffset; // Jumps straight to the first Normals value in the vertices array for an object
		in1 += normalOffset; // Jumps to the second Normals value
		in2 += normalOffset; // Jumps to the third Normals value

		vertices[in0]     += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1]     += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2]     += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vertLength; i++)
	{
		unsigned int normOffset = i * vertLength + normalOffset;
		glm::vec3 vec(vertices[normOffset], vertices[normOffset + 1], vertices[normOffset + 2]);
		vec = glm::normalize(vec);
		vertices[normOffset]     = vec.x;
		vertices[normOffset + 1] = vec.y;
		vertices[normOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	GLfloat planeVertices[] =
	{
		//    Positions      Tex Coords    Normals
		-1.0f, -0.5f,  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 1.0f,  0.5f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,

		 1.0f,  0.5f, -1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		-1.0f, -0.5f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f
	};

	unsigned int planeIndices[] =
	{
		0, 1, 2, // first triangle
		0, 2, 3  // second triangle
	};

	// Calculate the Normals using the `calcAverageNorms()` function
	calcAverageNorms(planeIndices, 6, planeVertices, 32, 8, 5);

	Mesh *planeMesh = new Mesh();
	planeMesh->CreateMesh(planeVertices, planeIndices, 32, 6);
	meshList.push_back(planeMesh);
	

	GLfloat cubeVerts[] =
	{
		//   Positions        Tex Coords   Normals
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f
	};

	unsigned int cubeIndices[] =
	{
		// Front Face
		0, 1, 2,
		2, 3, 0,

		// Right Face
		1, 5, 6,
		6, 2, 1,

		// Back Face
		7, 6, 5,
		5, 4, 7,

		// Left Face
		4, 0, 3,
		3, 7, 4,

		// Bottom Face
		4, 5, 1,
		1, 0, 4,

		// Top Face
		3, 2, 6,
		6, 7, 3
	};

	// Calculate the Normals for cubes
	calcAverageNorms(cubeIndices, 36, cubeVerts, 64, 8, 5);

	// Mouse pad shape
	Mesh *mousepadMesh = new Mesh();
	mousepadMesh->CreateMesh(cubeVerts, cubeIndices, 64, 36);
	meshList.push_back(mousepadMesh);

	GLfloat rectangleVerts[] = {
		// Positions          Tex Coords      Normals
		// Front Face
		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

		// Back Face
		-1.0f, -1.0f, -1.0f,  0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
	};

	unsigned int rectangleIndices[] = {
		// Front Face
		0, 1, 2,
		2, 3, 0,

		// Back Face
		4, 5, 6,
		6, 7, 4,

		// Right Face
		1, 5, 6,
		6, 2, 1,

		// Left Face
		4, 7, 3,
		3, 0, 4,

		// Top Face
		3, 2, 6,
		6, 7, 3,

		// Bottom Face
		4, 5, 1,
		1, 0, 4
	};

	calcAverageNorms(rectangleIndices, 36, rectangleVerts, 64, 8, 5);

	// Keyboard shape
	Mesh* keyboardMesh = new Mesh();
	keyboardMesh->CreateMesh(rectangleVerts, rectangleIndices, 64, 36);
	meshList.push_back(keyboardMesh);

	// Keyboard keys
	Mesh* keysMesh = new Mesh();
	keysMesh->CreateMesh(cubeVerts, cubeIndices, 64, 36);
	meshList.push_back(keysMesh);

	// TODO: Add verts/inds/meshes for a cylinder and sphere
	
	// Cylinder
	GLfloat cylinderVerts[] =
	{
		// Positions                Tex Coords   Normals
		// Base
		0.0f, 0.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

		// Side
		0.0f, 0.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		cos(0.0f), sin(0.0f), 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

		cos(0.0f), sin(0.0f), 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		cos(1.0f), sin(1.0f), 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

		// Top
		0.0f, 1.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f,           0.0f, 0.0f,  0.0f, 0.0f, 0.0f
	};

	GLuint cylinderIndices[] = {
		// Base
		0, 1, 2,
		0, 2, 3,

		// Side
		4, 5, 6,
		4, 6, 7,
		7, 6, 8,
		7, 8, 9,

		// Top
		10, 11, 12,
		10, 12, 13,
	};

	//calcAverageNorms(cylinderIndices, 24, cylinderVerts, 96, 8, 5);

	// Mic stand shape
	Mesh* micstandMesh = new Mesh();
	micstandMesh->CreateMesh(cylinderVerts, cylinderIndices, 96, 24);
	meshList.push_back(micstandMesh);

	// Cone shape removed

	// Sphere
	const int numLatitudeSegments = 16;
	const int numLongitudeSegments = 32;
	std::vector<GLfloat> sphereVerts;
	std::vector<GLuint> sphereIndices;

	float radius = 0.5f;
	for (int lat = 0; lat <= numLatitudeSegments; ++lat) {
		float theta = static_cast<float>(lat) * glm::pi<float>() / numLatitudeSegments;
		for (int lon = 0; lon <= numLongitudeSegments; ++lon) {
			float phi = static_cast<float>(lon) * 2.0f * glm::pi<float>() / numLongitudeSegments;
			float x = radius * sinf(theta) * cosf(phi);
			float y = radius * cosf(theta);
			float z = radius * sinf(theta) * sinf(phi);
			float u = static_cast<float>(lon) / numLongitudeSegments;
			float v = static_cast<float>(lat) / numLatitudeSegments;

			sphereVerts.push_back(x);
			sphereVerts.push_back(y);
			sphereVerts.push_back(z);
			sphereVerts.push_back(u);
			sphereVerts.push_back(v);
			sphereVerts.push_back(x);
			sphereVerts.push_back(y);
			sphereVerts.push_back(z);
		}
	}

	for (int lat = 0; lat < numLatitudeSegments; ++lat) {
		for (int lon = 0; lon < numLongitudeSegments; ++lon) {
			int currRow = lat * (numLongitudeSegments + 1);
			int nextRow = (lat + 1) * (numLongitudeSegments + 1);
			sphereIndices.push_back(currRow + lon);
			sphereIndices.push_back(nextRow + lon);
			sphereIndices.push_back(currRow + lon + 1);
			sphereIndices.push_back(nextRow + lon);
			sphereIndices.push_back(nextRow + lon + 1);
			sphereIndices.push_back(currRow + lon + 1);
		}
	}

	//calcAverageNorms(sphereIndices.data(), static_cast<int>(sphereIndices.size()), sphereVerts.data(), static_cast<int>(sphereVerts.size()), 8, 5);

	Mesh* sphereMesh = new Mesh();
	sphereMesh->CreateMesh(sphereVerts.data(), sphereIndices.data(), static_cast<int>(sphereVerts.size()), static_cast<int>(sphereIndices.size()));
	meshList.push_back(sphereMesh);

	// Full circle shape
	const int numSegments = 64; // Increase the number of segments for smoother circle
	radius = 0.5f;
	std::vector<GLfloat> fullCircleVerts;
	std::vector<GLuint> fullCircleIndices;

	for (int i = 0; i <= numSegments; ++i)
	{
		float angle = static_cast<float>(i) / numSegments * 2.0f * glm::pi<float>();
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		float u = static_cast<float>(i) / numSegments;
		float v = 0.5f; // Keep v-coordinate constant for full circle

		fullCircleVerts.push_back(x);
		fullCircleVerts.push_back(y);
		fullCircleVerts.push_back(0.0f);
		fullCircleVerts.push_back(u);
		fullCircleVerts.push_back(v);
		fullCircleVerts.push_back(0.0f);
		fullCircleVerts.push_back(0.0f);
		fullCircleVerts.push_back(1.0f);

		if (i > 0)
		{
			fullCircleIndices.push_back(0);
			fullCircleIndices.push_back(i);
			fullCircleIndices.push_back(i + 1);
		}
	}

	// Create the mesh for the full circle
	Mesh* fullCircleMesh = new Mesh();
	fullCircleMesh->CreateMesh(fullCircleVerts.data(), fullCircleIndices.data(),
		static_cast<int>(fullCircleVerts.size()),
		static_cast<int>(fullCircleIndices.size()));
	meshList.push_back(fullCircleMesh);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialize();


	// Function calls
	CreateObjects();
	CreateShaders();

	// position, worldup, yaw, pitch, move speed, turn speed (mouse control)
	camera = Camera(glm::vec3(0.0f, 0.5f, 2.5f), glm::vec3(0.0f, 2.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	// Textures for all objects
	planeTexture = Texture("Textures/woodTex.jpg");
	planeTexture.LoadTexture();

	keyboardTexture = Texture("Textures/blackTex.jpg");
	keyboardTexture.LoadTexture();

	mousepadTexture = Texture("Textures/designTex.jpg");
	mousepadTexture.LoadTexture();

	keycapTexture = Texture("Textures/grayTex.jpg");
	keycapTexture.LoadTexture();

	micstandTexture = Texture("Textures/blueTex.jpg");
	micstandTexture.LoadTexture();

	micTexture = Texture("Textures/meshTex.jpg");
	micTexture.LoadTexture();

	// Specular Lighting
	shinyMaterial = Material(1.0f, 16);
	dullMaterial = Material(0.3f, 4);

	// Lighting       r |   g |   b |  amb | dir x | dir y | dir z | intensity
	mainLight = Light(1.0f, 1.0f, 1.0f, 0.05f, 1.0f, 0.0f, -1.0f, 0.5f); // plain bright white light

	GLuint uniformProjection = 0, 
		   uniformModel = 0, 
		   uniformView = 0, 
		   uniformAmbientIntensity = 0, 
		   uniformAmbientColor = 0,
		   uniformDirection = 0,
		   uniformDiffuseIntensity = 0,
	       uniformEyePosition = 0,
		   uniformSpecularIntensity = 0,
		   uniformShininess = 0
		;


	glm::mat4 projection = glm::perspective(glm::radians(45.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);


	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // converts into a value in seconds -> (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		// Checks what keys are being pressed to move the camera
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());



		// Check for 'P' key press to toggle between orthographic and perspective views
		if (mainWindow.getsKeys()[GLFW_KEY_P])
		{
			isPerspective = !isPerspective;
		}

		// Set the projection matrix accordingly
		if (isPerspective)
		{
			projection = glm::perspective(glm::radians(45.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
		}
		else
		{
			// Set up an orthographic projection matrix
			GLfloat left = -1.0f;
			GLfloat right = 1.0f;
			GLfloat bottom = -1.0f;
			GLfloat top = 1.0f;
			GLfloat near = 0.1f;
			GLfloat far = 100.0f;

			projection = glm::ortho(left, right, bottom, top, near, far);
		}


		// Clear the window
		glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColor = shaderList[0].GetAmbientColorLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
		uniformDirection = shaderList[0].GetDirectionLocation();
		uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();


		// Use the lighting
		mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor, uniformDirection, uniformDiffuseIntensity);

		// The uniform projection and view only need to be set once as long as it's set before we draw
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model = glm::mat4(1.0f);


		// Render the plane
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
		model = glm::scale(model, glm::vec3(10.0f, 0.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		planeTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		// Render the mouse pad
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.5f, -2.0f, -1.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.05f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mousepadTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		// Render the keyboard
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, -0.89f, -1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.1f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		keyboardTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Render the keyboard keys
		glm::vec3 startPosition(-4.0f, -0.35f, -2.2f); // Starting position of the first cube
		
		const int rows = 4;
		const int cols = 10;
		const float cubeSpacing = 0.11f;

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < cols; col++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, startPosition + glm::vec3(col * (0.25f + cubeSpacing), -0.50f, row * (0.30f + cubeSpacing)));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(0.1f, -0.1f, -0.1f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			    keycapTexture.UseTexture();

				if (col == cols - 1 && row == rows - 1)
				{
					break;
				}

				meshList[2]->RenderMesh();
			}
		}

		// Render the mic stand
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, 0.0f, -3.5f));
		model = glm::scale(model, glm::vec3(0.2f, 3.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		micstandTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, 1.55f, -3.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 3.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		micstandTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// Render mic body (cone)
		/*model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, 1.55f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		micTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();*/

		// Render mic
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, 1.55f, -1.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		micTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		// Render micstand base
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.2f, -0.95f, -3.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		micstandTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		// Unassign the shader program when done
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}