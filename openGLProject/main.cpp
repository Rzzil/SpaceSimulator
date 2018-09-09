#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <sstream>//string stream
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION //so it wont go looking for the c or cpp file
#include "stb_image.h"

#include "Camera.h"


using namespace std;


bool wireFrame = false;

int selectedColour = 1;

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;

//Time management stuff
float deltaTime = 0.0f;//time between current frame and last frame
float lastFrame = 0.0f;//time of last frame


//window resize call back function prototype
void windowResizeCallBack(GLFWwindow* window, int width, int height);

//user inputs
void processInputs(GLFWwindow* window);


//mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//scroll wheel callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//init the game
GLFWwindow* GameInit();

//Load Up Image file
void LoadUpImage();



//Frames Per Second prototype
void showFPS(GLFWwindow* window);

//Vertex Shader Program Source Code
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n" //variable location[0], read value in of type vec3
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n\0";//need \0 to tell it its the end of the character string

//Fragment Shader Program Source Code
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n" //value passed on to next shader when done
"\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


void main()
{
	GLFWwindow *window = GameInit();

	Shader shaderProgram1("vertexShader1.txt", "fragmentShader1.txt");
	Shader shaderProgram2("vertexShader2.txt", "fragmentShader2.txt");
	Shader shaderProgram3("vertexShader3.txt", "fragmentShader3.txt");
	Shader shaderProgram4("cubeVertexShader.txt", "cubeFragmentShader.txt");

	//Compile Shader Source into shader programs
	//vertex shader first
	int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);//gives id for a vertex shader
	//give it the source code
	//params:
	//		shader id
	//		number of source strings
	//		source strings (1 or array)
	//		length of string or NULL if it ends in a \0
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	//compile the code on the gpu
	glCompileShader(vertexShaderID);

	//check for errors
	int success;
	char infoLog[512];
	//check compile status for our program, store result in success
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	//failed?
	if (!success)
	{
		//get error for the log and store in infoLog
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		cout << "Vertex shader error!: " << infoLog << endl;
		system("pause");
	}
	//Fragment shader
	int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	//failed?
	if (!success)
	{
		//get error for the log and store in infoLog
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		cout << "Fragment shader error!: " << infoLog << endl;
		system("pause");
	}
	//Create a Shader Program which links a bunch of shaders together for a full pipeline
	int shaderProgramID = glCreateProgram();
	//attach our shaders to this program
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	//link the attached shaders
	glLinkProgram(shaderProgramID);
	//check for linking errors
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		cout << "Shader Program Linking Error!: " << infoLog << endl;
		system("pause");
	}
	//our shaders were used to help make the full shader program, we dont need them individually anymore
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//3 points in X,Y,Z
	float vertices[] =
	{
		0.5f, -0.5f, 0, //bottom right point
		-0.5f, -0.5f, 0, //bottom left
		0.0f, 0.5f, 0 // top
	};

	float rectVertices[] =
	{
		//first triangle
		0.5f, 0.5f, 0.0f,//top right  0
		0.5f, -0.5f, 0.0f, //bottom right   1
		//-0.5f, 0.5f, 0.0f, //top left
		//second triangle
		//0.5f, -0.5f, 0.0f, //bottom right
		-0.5f, -0.5f, 0.0f, //bottom left  2
		-0.5f, 0.5f, 0.0f //top left  3

	};

	float polygon[] =
	{
		//first triangle
		-1.0f, 0.0f, 0.0f,//0
		0.0f, 0.0f, 0.0f,//1
		1.0f, 0.0f, 0.0f,//2
		-0.6f, 0.7f, 0.0f,//3
		0.6f, 0.7f, 0.0f,//4
		0.0f, 1.0f, 0.0f,//5
	};

	//the vertices order, treating each xyz as an index
	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	float rainbowTriangleVertices[] =
	{
		//x		y		z	r		g	b
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	float colourRectVertices[] =
	{
		// positions      // colors   // texture coords
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};

	float textureCubVertices[] =
	{
		//x		y		z	  texX	texY
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	};


	
	//CREATE VBO TO STORE VERTICES
	unsigned int textureRectVBO;
	glGenBuffers(1, &textureRectVBO);
	//CREATE EBO TO STORE CONNECTION INDEXES
	unsigned int textureRectEBO;
	glGenBuffers(1, &textureRectEBO);
	//CREATE VAO TO STORE OPERATIONS ON VBO
	unsigned int textureRectVAO;
	glGenVertexArrays(1, &textureRectVAO);
	//to work with this VAO bind it to make it the current one
	glBindVertexArray(textureRectVAO);
		//bind the buffer object to this vao
		glBindBuffer(GL_ARRAY_BUFFER, textureRectVBO);
		//give data to VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(colourRectVertices), colourRectVertices, GL_STATIC_DRAW);


		//bind elemental buffer object to this vao
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureRectEBO);
		//give indices to ebo
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//tell VAO which part of the VBO is for location = 0 of our vertex shader
		//Position(x,y,z)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//onto location = 1
		//Colour(R,G,B)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//last, location = 2
		//Texture Coordinate(S,T)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	//unbind our VAO
	glBindVertexArray(0);


	//generate a texture in gpu, return id
	unsigned int texture1ID;
	glGenTextures(1, &texture1ID);
	//we bind the texture to make it the one we're working on
	glBindTexture(GL_TEXTURE_2D, texture1ID);
	//set wrapping options(repeat texture if texture coordinates dont fully cover polygons)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//wraps on the s(x) axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//wraps on the t(y) axis
	//set filtering options
	//Suggestion use nearest neightbour for pixel art, use bilinear for pretty much everything else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR(bilinear) or GL_NEAREST for shrinking
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//for stretching


	//1. Vertex represents what data? position(xyz), colour(rgb), texture Coordinates(Ts, Tt)


	//2. Vertex Buffer Object? stores vertices in the GPU
	unsigned int cubeVBO;
	glGenBuffers(1, &cubeVBO);


	//3. Vertex Array Object? tries to describe the data in the VBO and relay it to the first shader
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCubVertices), textureCubVertices, GL_STATIC_DRAW);

	//xyz to location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texture coordinates to location = 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//unbind stuff
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Load up an image file
	LoadUpImage();
	
	//Generate a texture in our graphics card to work with
	unsigned int texture2ID;
	glGenTextures(1, &texture2ID); //generate 1 texture id and store in texture2ID
	glBindTexture(GL_TEXTURE_2D, texture2ID);//make this texutre the currently working texture, saying its a 2d texture (as opposed to 1d and 3d)
	
	//how will texture repeat on large surfaces
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//how wrap horizontally (S axis...)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//how to wrap vertically (T axis...)
	//how will texture deal with shrink and stretch
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//when shrinking texture use bilinear filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//use nearest neighbour filtering on stretch

	////Load up an image
	//unsigned char *imageData2 = stbi_load("airplane.png", &width, &height, &numberChannels, 0);

	//if (imageData2)
	//{
	//	//give the texture in our graphics card the data from this png file
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData2);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	cout << "failed to load airplane" << endl;
	//}
	////free image data from ram because theres a copy in the texture
	//stbi_image_free(imageData2);



	//generate vertex buffer objects in our GPU to store our vertices
	//if we only need 1
	unsigned int VBO; //we're gonna store the id of a vertex buffer object in this var
	glGenBuffers(1, &VBO); //params: how many vertex buffer objects to build and where to store their id's
	//if we need to generate multiple VBO's
	//unsigned int VBOs[2];
	//glGenBuffers(2, VBOs);

	//vbo for rect
	unsigned int VBO2;
	glGenBuffers(1, &VBO2);

	//vbo for rainbow triangle
	unsigned int VBO3;
	glGenBuffers(1, &VBO3);

	//Elemental Buffer Object
	//we're going to use this thing to hold the indices values to help describe
	//in which order to construct stuff out of a set of indices
	unsigned int EBO; //id of EBO we want to build in GPU
	glGenBuffers(1, &EBO); //generate buffer, return 1 id, store id in our EBO variable


	//generate Vertex Array Object, we bind vbo data to this thing aswell as a few specific function calls
	unsigned int VAO; //we'll store an id in this variable
	glGenVertexArrays(1, &VAO); //how many arrays to build, where to store ids(we just want 1, store id in VAO)

	//VAO2 was for rect
	unsigned int VAO2;
	glGenVertexArrays(1, &VAO2);

	//VAO3 for rainbow triangle VBO3
	unsigned int VAO3;
	glGenVertexArrays(1, &VAO3);
	

	//to start binding data to our VAO, we have to make our VAO active by binding it first
	glBindVertexArray(VAO);

	//bind the VBO we want to the current working array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//describe the VBO data and bind the vertices
	//params:
	//which buffer to bind to? (the current binded array buffer)
	//size of the data
	//the data
	//type of data: static, dynamic, stream
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//tell it how to feed this vertex data into the vertex shader
	//params:
	//0 = variable location in our vertex shader program(aPos)
	//3 = how many values make up a vertex in our data (1-4)
	//GL_FLOAT = what datatype to map to in the shader
	//GL_FALSE = 
	//3*sizeof(float) = Stride(how to find the next vertex), after current, the next is 3 floats up
	//(void*)0 = must be (void*) type..., but the zero is 'what is the starting index of your data'
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//enable this variable location slot in our vertex shader
	glEnableVertexAttribArray(0); //vec3 aPos

	//finished binding information to current VAO, set binding to nothing for safety
	glBindVertexArray(0);

	//VAO2 binding
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(polygon), polygon, GL_STATIC_DRAW);
	//bind our EBO buffer for use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//bind our indices to this buffer
	//param:	bufferType,              size of data,      data to store, draw type
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//bind VAO
	glBindVertexArray(VAO3);
	//things to do with buffer and vertex attributes will be binded to VAO3 now
		//bind buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		//give buffer the array of data
		glBufferData(GL_ARRAY_BUFFER, sizeof(rainbowTriangleVertices), rainbowTriangleVertices, GL_STATIC_DRAW);

		//tell it which parts of our rainbowTriangleVertices array are associated with vertex shaders
		//location = 0 variable aPos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//next, we want to tell it which parts of our array are the colour values to go into location = 1 variable aColour
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);
	glBindVertexArray(0);//unbind the VAO so we dont accidentally mess with our currently binded options

	//array of 10 cube positions
	/*glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};*/



	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		//update our time management stuff
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		//user inputs
		processInputs(window);
		//GROWTH
		//MINDSET!

		//RENDER STUFF
		//set openGL clear colour
		glClearColor(0.1255, 0.1412, 0.1608, 1);//r,g,b,a as floats, 0 to 1
		//clear screen AND clear Z depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram4.use();
		//bind VAO to draw with
		glBindVertexArray(cubeVAO);



		//set active textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2ID);

		//set texture uniforms: tell each texture uniform which texture slot to use
		glUniform1i(glGetUniformLocation(shaderProgram4.ID, "texture1"), 0);//GL_TEXTURE0
		glUniform1i(glGetUniformLocation(shaderProgram4.ID, "texture2"), 1);//GL_TEXTURE1


		//Coordinate systems:
		//cube's vertices are in LOCAL SPACE, we need to convert it to WORLD space
		//glm::mat4 model = glm::mat4(1.0f);
		//rotate our cube
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		//convert WORLD SPACE TO VIEW SPACE (adjust stuff based on where camera is looking)
		glm::mat4 view = camera.GetViewMatrix();//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));//push objects away to simulate moving camera backwards
		/*float radius = 5.0f;
		float camX = sin(glfwGetTime())*radius;
		float camZ = cos(glfwGetTime())*radius;*/

		//lookat			cameraPosition				target position				which way is up
		//view = glm::lookAt(glm::vec3(camX, 0, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));



		//projection matrix helps create the mathematical illusion of perspective
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		//to set uniform values on shader
		//glUniformMatrix4fv(glGetUniformLocation(shaderProgram4.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram4.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram4.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


		//loop through to create a model matrix per position
		//for (int i = 0; i < 10; i++)
		//{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
			/*if (i % 2 == 0)
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
			else
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			*/
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram4.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);//starting at stride0, draw 36 rows of vertex data
		//}

		
		//Input for window
		glfwPollEvents();

		//swap render buffers with this loops rendered scene
		glfwSwapBuffers(window);

		showFPS(window);
	}

	//optional: de-allocate all resources
	glDeleteVertexArrays(1, &VAO);//params: how many, thing with ids(unsigned int, or array of)
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);
	//glDeleteBuffers(2, VBOs); //example of deleting 2 VBO ids from the VBOs array


	glfwTerminate();
}

//window resize call back function prototype
void windowResizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//user inputs
void processInputs(GLFWwindow* window)
{
	//if esc pressed, set window to 'should close'
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		//flip wiremode value
		wireFrame = !wireFrame;
		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		selectedColour = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		selectedColour = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		selectedColour = 3;

	//camera walking
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void showFPS(GLFWwindow* window)
{
	//static function variables are declared 
	//once per project run, so these 2 lines of 
	//code run once and then the variables persist
	//until the end of the program
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();
	//getTime returns seconds since startup

	elapsedSeconds = currentSeconds - previousSeconds;
	if (elapsedSeconds > 0.25){
		previousSeconds = currentSeconds;
		double fps = frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		stringstream ss;
		ss.precision(3);//3 decimal places
		ss << fixed << "Game1 FPS: " << fps << " Frame Time: " << msPerFrame << "(ms)";

		glfwSetWindowTitle(window, ss.str().c_str());
		frameCount = 0;
	}
	frameCount++;
}

//mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		//helps not make a massive camera rotation jump when mouse first touches the screen
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//scroll wheel callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


GLFWwindow* GameInit()
{
	glfwInit(); 
	//tell glfw that we want to work with openGL 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //the first 3 of 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //the .3 of 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core profile

																   //build window
	GLFWwindow *window = glfwCreateWindow(800, 600, "My first OpenGL thing", NULL, NULL);
	//if it fails
	if (window == NULL)
	{
		//try report error
		cout << "failed to create window" << endl;
		glfwTerminate(); //cleanup glfw stuff
		system("pause");
	}
	//make this window the current one
	glfwMakeContextCurrent(window);

	//initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//if this fails, then
		cout << "GLAD failed to initialise" << endl;
		glfwTerminate(); //cleanup glfw stuff
		system("pause");
	}

	//set up openGL viewport x,y,w,h
	glViewport(0, 0, 800, 600);//you dont have to use the full window for openGL but we will

							   //set z buffering on
	glEnable(GL_DEPTH_TEST);


	//load images in, flip them
	stbi_set_flip_vertically_on_load(true);




	//add window resize callback, params: window to check events on, function to call
	//when it resizes
	glfwSetFramebufferSizeCallback(window, windowResizeCallBack);

	//register our mouse callbacks
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//hide cursor and lock mouse within window area
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

void LoadUpImage()
{
	//LOAD UP IMAGE FILE (JPEG FIRST)
	int width, height, numberChannels; //as we load an image, we'll get values from it to fill these in
	unsigned char *image1Data = stbi_load("Assets/Earth.jpg", &width, &height, &numberChannels, 0);
	//if it loaded
	if (image1Data)
	{
		cout << "Success! Image is " << width << " by " << height << " pixels" << endl;
		//Lets associate our texture with this image data
		//params:
		//	texture type
		//	0 = mipmap level(if you want to set manually, zero = nah)
		//	GL_RGB = format we want to store the texture data in
		//	width/height = size of texture
		//	0 = must always be zero, some lagacy shit
		//	GL_RGB = if jpg, its considered RGB
		//	GL_UNSIGNED_BYTE = how the data has been loaded up for image1Data (unsigned char, char = byte)
		//	image1Data = our image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image1Data);
		//note: above tells openGL how to take something from ram and store it in vram against our textureID

		//generate mipmaps for this texture
		glGenerateMipmap(GL_TEXTURE_2D);
		//generates a bunch of smaller versions of the texture to be used at great distances so save on
		//processing
	}
	else
	{
		cout << "Image load failed!" << endl;
	}
	//cleanup image memory
	stbi_image_free(image1Data);
}