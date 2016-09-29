//#include "pch.h"
//#include "SimpleRenderer.h"
//#include "MathHelper.h"
//#include "vtkPolyDataNormals.h"
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <cstdlib>
//// These are used by the shader compilation methods.
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include "PolyDataReceiver.cpp"
//#include "esUtil.h"
//
//
//GLuint LoadShader(GLenum type, const char *shaderSrc)
//{
//	GLuint shader;
//	GLint compiled;
//
//	// Create the shader object
//	shader = glCreateShader(type);
//
//	if (shader == 0)
//	{
//		return 0;
//	}
//
//	// Load the shader source
//	glShaderSource(shader, 1, &shaderSrc, NULL);
//
//	// Compile the shader
//	glCompileShader(shader);
//
//	// Check the compile status
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
//
//	if (!compiled)
//	{
//		GLint infoLen = 0;
//
//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
//
//		if (infoLen > 1)
//		{
//			char *infoLog = malloc(sizeof(char) * infoLen);
//
//			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
//			esLogMessage("Error compiling shader:\n%s\n", infoLog);
//
//			free(infoLog);
//		}
//
//		glDeleteShader(shader);
//		return 0;
//	}
//
//	return shader;
//
//}