/*
	Proyecto Final Computo Grafico e Interacción Humano Computadora
	Profesor: Arturo 
	Grupo:
	Integrantes: 
				Gonzalez Romero Daniel Vicente - 317071201
				Hernandez Lara Jesus Eduardo - 316143671

*/

//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


//Texturas 
Texture pisoTexture;


//Modelos 
Model Poste_M;
Model Puerta_M;
Model Luciernaga_M;
Model BaseCarrusel_M;
Model Caballos_M;
Model Planta_M;
Model Banca_M;
Model Personaje_M;
Model CoffeeShop_M;
Model MesaBancos_M;
Model MesaSillas_M;
Model Sombrillas_M;
Model Sillon_M;
Model Arboles_M;
Model CaballoTorso_M;
Model CaballoPatasT_M;
Model CaballoPatasD_M;
Model CaballoCabeza_M;
Model Niño_M;
Model Licoreria_M;
Model PandaTorso_M;
Model PandaPataDI_M;
Model PandaPataDD_M;
Model PandaPataTI_M;
Model PandaPataTD_M;
Model PandaCabeza_M;



// Variables para animacion
float giro_carrusel;
float giro_carrusel_offset;

float mov_caballos;
float mov_caballos_offset;

float CaballoTorso;
float CaballoTorso_offset;
float CaballoPatasT;
float CaballoPatasT_offset;
float CaballoPatasD;
float CaballoPatasD_offset;
float CaballoCabeza;
float CaballoCabeza_offset;

float PandaTorso;
float PandaTorso_offset;
float PandaRotarPatas;
float PandaRotarPatas_offset;
float PandaCabeza;
float PandaCabeza_offset;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight arr2pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {					//Es la normal de donde se va a iluminar (de arriba hacia abajo)
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		1.0f, -1.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		1.0f, -1.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		1.0f, -1.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		1.0f, -1.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		1.0f, -1.0f, -1.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		1.0f, -1.0f, -1.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		1.0f, -1.0f, -1.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		1.0f, -1.0f, -1.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();


	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);


	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();



	Poste_M = Model();
	//Poste_M.LoadModel("Models/lamp.obj");
	Puerta_M = Model();
	//Puerta_M.LoadModel("Models/puerta_previo.obj");
	Luciernaga_M = Model();
	//Luciernaga_M.LoadModel("Models/luciernaga.obj");
	BaseCarrusel_M = Model();
	//BaseCarrusel_M.LoadModel("Models/base_carrusel.obj");
	Caballos_M = Model();
	//Caballos_M.LoadModel("Models/caballos.obj");

		//Modelos ambientacion general
	Planta_M = Model();
	Planta_M.LoadModel("Models/plantas.obj"); // Con Cubo en el origen para referencia
	Banca_M = Model();
	Banca_M.LoadModel("Models/bancas.obj");
	Arboles_M = Model();
	Arboles_M.LoadModel("Models/Arboles.obj");

		//Modelos Tienda 1
	CoffeeShop_M = Model();
	CoffeeShop_M.LoadModel("Models/coffe_shop.obj");
	MesaBancos_M = Model();
	MesaBancos_M.LoadModel("Models/mesa_bancos.obj");
	MesaSillas_M = Model();
	MesaSillas_M.LoadModel("Models/mesa_sillas.obj");
	Sombrillas_M = Model();
	Sombrillas_M.LoadModel("Models/sombrillas.obj");
	Sillon_M = Model();
	Sillon_M.LoadModel("Models/sofa_cafe.obj");

		//Modelos Tienda 2
	Licoreria_M = Model();
	Licoreria_M.LoadModel("Models/licoreria.obj");

		// Modelos Caballo 
	CaballoTorso_M = Model();
	CaballoTorso_M.LoadModel("Models/TorsoCaballo.obj");
	CaballoPatasT_M = Model();
	CaballoPatasT_M.LoadModel("Models/PatasTraserasC.obj");
	CaballoPatasD_M = Model();
	CaballoPatasD_M.LoadModel("Models/PatasDelanterasC.obj");
	CaballoCabeza_M = Model();
	CaballoCabeza_M.LoadModel("Models/CabezaCaballo.obj");
	Niño_M = Model();
	Niño_M.LoadModel("Models/Niño.obj");

		// Modelos Panda
	PandaTorso_M = Model();
	PandaTorso_M.LoadModel("Models/Panda/TorsoPanda.obj");
	PandaPataDD_M = Model();
	PandaPataDD_M.LoadModel("Models/Panda/PataDelanteraDP.obj");
	PandaPataDI_M = Model();
	PandaPataDI_M.LoadModel("Models/Panda/PataDelanteraIP.obj");
	PandaPataTD_M = Model();
	PandaPataTD_M.LoadModel("Models/Panda/PataTraseraDP.obj");
	PandaPataTI_M = Model();
	PandaPataTI_M.LoadModel("Models/Panda/PataTraseraIP.obj");
	PandaCabeza_M = Model();
	PandaCabeza_M.LoadModel("Models/Panda/CabezaPanda.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/city_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/city_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/city_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/city_up.tga");
	skyboxFaces.push_back("Textures/Skybox/city_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/city_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.7f, 0.0f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 3.0f, 4.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	arr2pointLights[0] = PointLight(0.92f, 0.92f, 0.32f,
		0.0f, 1.0f,
		0.0f, 2.15f, 9.5f,
		0.1f, 0.1f, 0.1f);
	/*pointLightCount++;*/
	//no agregamos una nueva pointLigth, ya que el segundo arreglo tiene el mismo tamaño



	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	

	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	
	//Loop mientras no se cierra la ventana

	// Variables de animacion
	giro_carrusel = 0.0f;
	giro_carrusel_offset = 10.0f;
	mov_caballos = 0.0f;
	mov_caballos_offset = 10.0f;

	CaballoTorso = 0.0f;
	CaballoTorso_offset = 0.005f;
	CaballoPatasT = 0.0f;
	CaballoPatasT_offset = -0.05f;
	CaballoPatasD = 0.0f;
	CaballoPatasD_offset = 0.05f;
	CaballoCabeza = 0.0f;
	CaballoCabeza_offset = 0.05f;

	PandaTorso = 0.0f;
	PandaTorso_offset = 0.004f;
	PandaRotarPatas = 0.0f;
	PandaRotarPatas_offset = 0.05f;
	PandaCabeza = 0.0f;
	PandaCabeza_offset = 0.05f;

	
	

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;


		//Animación caballos carrusel 
		if (mov_caballos_offset > 360)
			mov_caballos_offset = 10.5f;

		giro_carrusel += 0.02*giro_carrusel_offset * deltaTime;
		mov_caballos_offset += 0.5f;


		// Prueba Animación Caballo Carrito
		CaballoCabeza += CaballoCabeza_offset;
		if (CaballoCabeza >= 25.0f || CaballoCabeza <= -25.0f)
			CaballoCabeza_offset = (CaballoCabeza_offset * (- 1));

		CaballoPatasT += CaballoPatasT_offset;
		if (CaballoPatasT >= 45.0f || CaballoPatasT <= -45.0f)
			CaballoPatasT_offset = (CaballoPatasT_offset * (-1));

		CaballoPatasD += CaballoPatasD_offset;
		if (CaballoPatasD >= 45.0f || CaballoPatasD <= -45.0f)
			CaballoPatasD_offset = (CaballoPatasD_offset * (-1));
			
		CaballoTorso += 0.006f;


		// Prueba Animacion Panda carrito
		PandaCabeza += PandaCabeza_offset;
		if (PandaCabeza >= 35.0f || PandaCabeza <= -35.0f)
			PandaCabeza_offset = (PandaCabeza_offset * (-1));

		PandaRotarPatas += PandaRotarPatas_offset;
		if (PandaRotarPatas >= 35.0f || PandaRotarPatas <= -35.0f)
			PandaRotarPatas_offset = (PandaRotarPatas_offset * (-1));

		PandaTorso += 0.004f;




		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Dibuja el skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Luces
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		shaderList[0].SetDirectionalLight(&mainLight);	// Luz ambiental
		
		
		// Matrices de transformaciones
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelAux_Carrusel(1.0f);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


		// Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();
		


		// Base Carrusel
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.15f, 2.0f));
		model = glm::rotate(model, glm::radians(giro_carrusel), glm::vec3(0.0f, 1.0f, 0.0f));
		modelAux_Carrusel = model;
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//BaseCarrusel_M.RenderModel();

		// Caballos Carrusel
		model = modelAux_Carrusel;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f + glm::radians(100*sin(mov_caballos_offset*0.008)), 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Caballos_M.RenderModel();

		//Plantas
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Planta_M.RenderModel();

		//Bancas
		Banca_M.RenderModel();
		

		//Coffee shop
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-50.0f, 0.0f, -80.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CoffeeShop_M.RenderModel();


		//Mesas afuera
		//model = glm::mat4(1.0f);
		Sombrillas_M.RenderModel();
		
		//Mesas dentro de la cafetería
		MesaSillas_M.RenderModel();
		
		//Sillón de la cafetería
		Sillon_M.RenderModel();



		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-40.0f, -2.0f, -80.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MesaBancos_M.RenderModel();

		//puerta
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, -8.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Puerta_M.RenderModel();
		spotLights[1].SetPos(glm::vec3(45.0f, 13.0f, -8.0f));



		// Panda

			//Torso
		model = glm::mat4(1.0f);
		modelaux = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(190.0f, 4.0f, (PandaTorso - 190.0f)));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaTorso_M.RenderModel();

			//Patas Traseras
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, -0.17f, -0.55f));
		model = glm::rotate(model, glm::radians(PandaRotarPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaPataTI_M.RenderModel();

		model = glm::translate(model, glm::vec3(-0.39f, 0.0f, -0.08f));
		model = glm::rotate(model, glm::radians(PandaRotarPatas*(-1)+5), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaPataTD_M.RenderModel();

			//Patas Delanteras
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.34f, -0.3f, 0.53f));
		model = glm::rotate(model, glm::radians(PandaRotarPatas), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaPataDI_M.RenderModel();

		model = glm::translate(model, glm::vec3(-0.6f, -0.04f, -0.13f));
		model = glm::rotate(model, glm::radians(PandaRotarPatas*(-1)+5), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaPataDD_M.RenderModel();

			// Cabeza
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f));
		model = glm::rotate(model, glm::radians(PandaCabeza), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PandaCabeza_M.RenderModel();

			// Niño montado
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.126f, 0.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Niño_M.RenderModel();

		
		




		// Blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		
		// Arboles (Se dibuja aqui ya que los planos que lo conforman necesitan transparencia)
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arboles_M.RenderModel();


		//Licoreria
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, -150.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Licoreria_M.RenderModel();



		// Caballo

		// Torso
		model = glm::mat4(1.0f);
		modelaux = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(170.0f, 6.2f, (-170.0f + CaballoTorso)));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CaballoTorso_M.RenderModel();

		// Patas Traseras
		model = glm::translate(model, glm::vec3(0.0f, -0.6f, -3.2f));
		model = glm::rotate(model, glm::radians(CaballoPatasT), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CaballoPatasT_M.RenderModel();

		// Patas Delanteras
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.1f, -1.4f, 1.9f ));
		model = glm::rotate(model, glm::radians((CaballoPatasD)), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CaballoPatasD_M.RenderModel();

		// Cabeza
		model = modelaux;
		model = glm::rotate(model, glm::radians(CaballoCabeza), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.1f, 1.35f, 2.95f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CaballoCabeza_M.RenderModel();

		// Niño montado
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Niño_M.RenderModel();
		

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
