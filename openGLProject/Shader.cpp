#include "Shader.h"



Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//1. retrieve vertex/fragment source code from filePath
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;


	//make sure ifstream objects can throw exceptions
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		//read files buffer contents into string streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cout << "Shader file not successfully read" << endl;
	}
	//convert string to character string(char*)
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	//2. compile shaders
	unsigned int vertexShaderID, fragmentShaderID;
	int success;
	//vertex shader
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vShaderCode, NULL);
	glCompileShader(vertexShaderID);
	checkCompileErrors(vertexShaderID, "VERTEX");
	//fragment program
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShaderID);
	checkCompileErrors(fragmentShaderID, "FRAGMENT");
	//build shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShaderID);
	glAttachShader(ID, fragmentShaderID);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	//delete individual shaders
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

}
void Shader::use()
{
	glUseProgram(ID);
}
void Shader::checkCompileErrors(unsigned int shaderID, string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		//check what error was for individual shaders being compiled
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
			cout << "Shader Compile Error: " << infoLog << endl;
		}
	}
	else
	{
		//check what linking errors there were on the shader program
		glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
			cout << "Program Linking Error: " << infoLog << endl;
		}
	}
}
