#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"
#include <random>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

//Algunos offsets
float xOffset = -960.0f;
float yOffset = 9472.0f;
float zOffset = -10944.0f;

// Camera
Camera  camera(glm::vec3(955.0f + xOffset, -9615.0f + yOffset, 11074.0f + zOffset));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float movCamera = 0.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(958.27f + xOffset, -9313.07f + yOffset, 11284.16f + zOffset - 200.0f),
	glm::vec3(958.27f + xOffset, -9588.88f + yOffset, 11284.16f + zOffset - 200.0f),
	glm::vec3(783.8521f + xOffset - 80.0f, -9623.662f + yOffset, 10999.195f + zOffset),
	glm::vec3(0,0,0)
};

glm::vec3 pointLightDiffuse[] = {
	glm::vec3(1.0f, 1.0f, 0.5f),
	glm::vec3(1.0f, 1.0f, 0.5f),
	glm::vec3(0.2f, 1.0f, 0.2f),
	glm::vec3(0,0,0)
};

glm::vec3 pointLightSpecular[] = {
	glm::vec3(1.0f, 1.0f, 0.5f),
	glm::vec3(1.0f, 1.0f, 0.5f),
	glm::vec3(0.2f, 1.0f, 0.2f),
	glm::vec3(0,0,0)
};

glm::vec3 spotlightPosition(783.8521f + xOffset, -9623.662f + yOffset, 10999.195f + zOffset);

glm::vec3 LightP1;

glm::vec3 posicionObus(0.0f, 0.0f, -2.0f);
glm::vec3 velocidad(0.0f, 0.0f, 0.0f);
float anguloObus = glm::radians(30.0f);
float tiempo_disparo = 0.0f;

std::random_device rd;
std::mt19937 e2(rd());
std::uniform_real_distribution<float> vel(200.0f, 5000.0f);


void saveFrame(void)
{

	printf("posx %f\n", posX);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();




	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	//printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lightingShader_cuarto("Shaders/lighting_cuarto.vs", "Shaders/lighting_cuarto.frag");
	Shader lightingShader_exterior("Shaders/lighting_exterior.vs", "Shaders/lighting_exterior.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	Shader modelShader("Shaders/modelLoading.vs", "Shaders/modelLoading.frag");

	//Objetos de la escena
	Model armario((char*)"Models/Siete_Objetos/Armario/armario.obj");
	Model caja_armas((char*)"Models/Siete_Objetos/Caja_Armas/caja_armas.obj");
	Model cama((char*)"Models/Siete_Objetos/Cama/cama.obj");
	Model escritorio((char*)"Models/Siete_Objetos/Escritorio/escritorio.obj");
	Model planta((char*)"Models/Siete_Objetos/Planta/planta.obj");
	Model sillon((char*)"Models/Siete_Objetos/Sillon/sillon.obj");
	Model terminal((char*)"Models/Siete_Objetos/Terminal/terminal_2.obj");

	Model habitacion_centro((char*)"Models/Habitacion/habitacion_centro.obj");
	Model pared_centro((char*)"Models/Habitacion/pared_centro.obj");
	Model pared_entrada((char*)"Models/Habitacion/pared_entrada.obj");
	Model pared_esquina((char*)"Models/Habitacion/pared_esquina.obj");
	Model puerta((char*)"Models/Habitacion/puerta.obj");
	Model lampara((char*)"Models/Habitacion/lampara.obj");

	Model lucky_38((char*)"Models/Edificio/Lucky_38/lucky_38.obj");

	Model desierto((char*)"Models/Terreno/desierto.obj");

	//Modelos de animaciones complejas
	Model avion("Models/Animaciones/Complejas/B29/B29_hanger.obj");
	Model artilleria("Models/Animaciones/Complejas/Artilleria/Artillery.obj");
	Model obus("Models/Animaciones/Complejas/Obus/obus.obj");

	//Animaciones por huesos
	ModelAnim ghoul("Models/Animaciones/Simples/ghoul_zombie.dae");
	ghoul.initShaders(animShader.Program);
	ModelAnim ranger_1("Models/Animaciones/Simples/ranger_1_acostado.dae");
	ranger_1.initShaders(animShader.Program);
	ModelAnim ranger_2("Models/Animaciones/Simples/ranger_2_sentado.dae");
	ranger_2.initShaders(animShader.Program);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right_2.jpg");
	faces.push_back("SkyBox/left_2.jpg");
	faces.push_back("SkyBox/back_2.jpg");
	faces.push_back("SkyBox/front.jpg");
	faces.push_back("SkyBox/top.jpg");
	faces.push_back("SkyBox/bottom.jpg");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 70000.0f);
	glm::mat4 projection_cuarto = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 70000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Draw skybox as last
		glDepthFunc(GL_LEQUAL);
		SkyBoxshader.Use();
		auto view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);



		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader_cuarto.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader_cuarto.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		//glUniform3f(glGetUniformLocation(lightingShader_exterior.Program, "dirLight.direction"), 0.0f, 0.0f, -1.0f);
		//glUniform3f(glGetUniformLocation(lightingShader_exterior.Program, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
		//glUniform3f(glGetUniformLocation(lightingShader_exterior.Program, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(lightingShader_exterior.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].specular"), 1.0f, 1.0f, 0.5f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.5f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].diffuse"), 0.2f, 1.0f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].specular"), 0.1f, 0.5f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].linear"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "pointLights[2].quadratic"), 1.8f);

		//// Point light 4
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		//// SpotLight
		//glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		//glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.position"), spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.direction"), -1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.diffuse"), 0.2f, 1.0f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.specular"), 0.1f, 0.5f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.linear"), 0.014f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.quadratic"), 0.0007f);
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.cutOff"), glm::cos(glm::radians(25.5f)));
		glUniform1f(glGetUniformLocation(lightingShader_cuarto.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(30.0f)));

		//// Set material properties
		//glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		//glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader_cuarto.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader_cuarto.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader_cuarto.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_cuarto));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		modelShader.Use();
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model(1);

		lightingShader_cuarto.Use();

		//Habitacion
		{
			//Habitación centro
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(960.0f + xOffset, -9472.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			habitacion_centro.Draw(lightingShader_cuarto);

			//Pared esquina
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(704.0f + xOffset, -9216.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_esquina.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1216.0f + xOffset, -9216.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_esquina.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1216.0f + xOffset, -9728.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_esquina.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(704.0f + xOffset, -9728.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_esquina.Draw(lightingShader_cuarto);

			//Pared centro
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(704.0f + xOffset, -9472.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_centro.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(960.0f + xOffset, -9216.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_centro.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1216.0f + xOffset, -9472.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_centro.Draw(lightingShader_cuarto);

			//Pared puerta
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(960.0f + xOffset, -9728.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			pared_entrada.Draw(lightingShader_cuarto);

			//Puerta
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(960.0f + xOffset, -9856.0f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			puerta.Draw(lightingShader_cuarto);

			//Lamparas
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(958.27f + xOffset, -9313.07f + yOffset, 11284.16f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			lampara.Draw(lightingShader_cuarto);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(958.27f + xOffset, -9588.88f + yOffset, 11284.16f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			lampara.Draw(lightingShader_cuarto);

		}

		//7 Objetos
		{
			//Armario
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(641.0227f + xOffset, -9247.679f + yOffset, 10944.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-294.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			armario.Draw(lightingShader_cuarto);

			//Caja de armas
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(620.0f + xOffset, -9427.0f + yOffset, 10945.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			caja_armas.Draw(lightingShader_cuarto);

			//Cama
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(953.0f + xOffset, -9204.0f + yOffset, 10974.0f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			cama.Draw(lightingShader_cuarto);

			//Escritorio
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(786.6762f + xOffset, -9630.152f + yOffset, 10943.914f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			escritorio.Draw(lightingShader_cuarto);

			//Planta
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(674.3022f + xOffset, -9163.861f + yOffset, 10944.109f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			planta.Draw(lightingShader_cuarto);

			//Sillon
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1124.4843f + xOffset, -9790.478f + yOffset, 10978.414f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			sillon.Draw(lightingShader_cuarto);

			//Terminal
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(783.8521f + xOffset, -9623.662f + yOffset, 10999.195f + zOffset));
			//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader_cuarto.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			terminal.Draw(lightingShader_cuarto);

		}

		//Animaciones por huesos
		animShader.Use();
		{
			modelLoc = glGetUniformLocation(animShader.Program, "model");
			viewLoc = glGetUniformLocation(animShader.Program, "view");
			projLoc = glGetUniformLocation(animShader.Program, "projection");

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
			glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
			glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.5, 0.5, 0.25);
			glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 1.0f, 1.0f, 0.5f);
			glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 1.0f, 1.0f, 0.5f);
			glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), 0.0f, -1.0f, -0.0f);

			//Ranger acostado
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(953.0f + xOffset - 25.0f, -9204.0f + yOffset + 6.0f, 10974.0f + zOffset - 70.0f));
			model = glm::rotate(model, glm::radians(80.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			ranger_1.Draw(animShader);

			//Ranger sentado
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1124.4843f + xOffset - 30.0f, -9790.478f + yOffset + 20.0f, 10978.414f + zOffset - 35.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			ranger_2.Draw(animShader);

			//Ghoul
			glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 1.0f, 1.0f, 1.0f);
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(1124.4843f + xOffset, -15000.0f + yOffset, 1100.0f + zOffset));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			ghoul.Draw(animShader);
		}

		modelShader.Use();
		//Terreno
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					model = glm::mat4(1);
					model = glm::translate(model, glm::vec3(958.27f + xOffset, -9488.88f + yOffset, 11284.16f + zOffset));
					model = glm::scale(model, glm::vec3(5000.0f * 2.0f));
					model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.1f));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::translate(model, glm::vec3(-3.44f + 3.43f * i, 0.02f, -3.44f + 3.43 * j));
					glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
					desierto.Draw(modelShader);
				}
			}
		}

		//Avion
		{
			glm::vec3 centro(958.27f + xOffset, -9488.88f - 80.0f + yOffset, 11284.16f + zOffset);
			float radio = 10000.0f;

			float frecuencia = 0.25f;

			float time = glfwGetTime();

			glm::vec3 posicion_rel(radio * std::cos(frecuencia * time), radio * std::sin(frecuencia * time), 0.0f);
			glm::vec3 posicion_abs = centro + posicion_rel;

			float angle = frecuencia * time;

			model = glm::mat4(1);
			model = glm::translate(model, posicion_abs);
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, angle + glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			avion.Draw(modelShader);

		}

		//Artilleria
		{
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(22267.0f, 16780.0f, -10000.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(5.0f));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			artilleria.Draw(modelShader);

			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(21289.7f - posicionObus.x, 16781.5f, -8854.64f + posicionObus.z));
			model = glm::rotate(model, glm::radians(90.0f) + anguloObus, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(60.0f));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			obus.Draw(modelShader);

		}

		//Edificio
		{
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(958.27f + xOffset, -9488.88f - 80.0f + yOffset, 11284.16f + zOffset));
			model = glm::scale(model, glm::vec3(75.0f));
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -155.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			lucky_38.Draw(modelShader);
		}

		//std::cout << camera.GetPosition().x << " " << camera.GetPosition().y << " " << camera.GetPosition().z << std::endl;

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{

		//Movimiento del personaje

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex>FrameIndex - 2)	//end of total animation?
				{
					printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posX += KeyFrame[playIndex].incX;
				posY += KeyFrame[playIndex].incY;
				posZ += KeyFrame[playIndex].incZ;

				rotRodIzq += KeyFrame[playIndex].rotInc;

				i_curr_steps++;
			}

		}
	}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	//if (keys[GLFW_KEY_L])
	//{
	//	if (play == false && (FrameIndex > 1))
	//	{

	//		resetElements();
	//		//First Interpolation				
	//		interpolation();

	//		play = true;
	//		playIndex = 0;
	//		i_curr_steps = 0;
	//	}
	//	else
	//	{
	//		play = false;
	//	}

	//}

	//if (keys[GLFW_KEY_K])
	//{
	//	if (FrameIndex<MAX_FRAMES)
	//	{
	//		saveFrame();
	//	}

	//	rot =-25.0f;//Variable que maneja el giro de la camara

	//}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	//if (keys[GLFW_KEY_SPACE])
	//{
	//	active = !active;
	//	if (active)
	//		LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
	//	else
	//		LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	//}

	if (keys[GLFW_KEY_SPACE])
	{
		camera.SetSpeed(60.0f * 128.0f);
	}
	else
	{
		camera.SetSpeed(60.0f * 4.0f);
	}

}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{
		
		movCamera = 0.01f;//Manda una velocidad de 0.01 a la camara automatica

	}

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	//Tiro parabolico
	if (posicionObus.z < -200.0f || keys[GLFW_KEY_E])
	{
		posicionObus = glm::vec3(0.0f, 0.0f, 0.0f);
		anguloObus = glm::radians(30.0f);

		float velocidadInicial = vel(e2);
		velocidad.x = velocidadInicial * std::cos(anguloObus);
		velocidad.y = 0.0f;
		velocidad.z = velocidadInicial * std::sin(anguloObus);

		tiempo_disparo = 0.0f;

		//std::cout << "Velocidad del obus: " << velocidadInicial << "u/s" << std::endl;

	}

	posicionObus.x = velocidad.x * tiempo_disparo;
	posicionObus.z = velocidad.z * tiempo_disparo - 0.5f * 9.78f * std::pow(tiempo_disparo, 2.0f);

	float vel_x = velocidad.x;
	float vel_z = velocidad.z - 9.78 * tiempo_disparo;

	anguloObus = atan(vel_z / vel_x);

	tiempo_disparo += deltaTime * 10.0f;

	//std::cout << "x=" << posicionObus.x << " z=" << posicionObus.z << " t=" << tiempo_disparo << " vx=" << vel_x << " vz=" << vel_z << " a=" << anguloObus << std::endl;


}