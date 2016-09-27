//
// This file is used by the template to render a basic scene using GL.
//

#include "pch.h"
#include "SimpleRenderer.h"
#include "MathHelper.h"
#include "vtkPolyDataNormals.h"
#include <shader.hpp>

// These are used by the shader compilation methods.
#include <vector>
#include <iostream>
#include <fstream>
#include "PolyDataReceiver.cpp"

using namespace Platform;

using namespace AppForOpenGLES1;

#define STRING(s) #s

GLuint CompileShader(GLenum type, const std::string &source)
{
	GLuint shader = glCreateShader(type);

	const char *sourceArray[1] = { source.c_str() };
	glShaderSource(shader, 1, sourceArray, NULL);
	glCompileShader(shader);

	GLint compileResult;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == 0)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);
		glGetShaderInfoLog(shader, (GLsizei)infoLog.size(), NULL, infoLog.data());

		std::wstring errorMessage = std::wstring(L"Shader compilation failed: ");
		errorMessage += std::wstring(infoLog.begin(), infoLog.end());

		throw Exception::CreateException(E_FAIL, ref new Platform::String(errorMessage.c_str()));
	}

	return shader;
}

GLuint CompileProgram(const std::string &vsSource, const std::string &fsSource)
{
	GLuint program = glCreateProgram();

	if (program == 0)
	{
		throw Exception::CreateException(E_FAIL, L"Program creation failed");
	}

	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSource);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSource);

	if (vs == 0 || fs == 0)
	{
		glDeleteShader(fs);
		glDeleteShader(vs);
		glDeleteProgram(program);
		return 0;
	}

	glAttachShader(program, vs);
	glDeleteShader(vs);

	glAttachShader(program, fs);
	glDeleteShader(fs);

	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

		std::wstring errorMessage = std::wstring(L"Program link failed: ");
		errorMessage += std::wstring(infoLog.begin(), infoLog.end());

		throw Exception::CreateException(E_FAIL, ref new Platform::String(errorMessage.c_str()));
	}

	return program;
}

SimpleRenderer::SimpleRenderer(bool isHolographic) :
	mWindowWidth(1268),
	mWindowHeight(720),
	mDrawCount(0)
{
	StartClient();
	localMutex->Lock();
	while (!receivedData)
	{
		conditionVar->Wait(localMutex);
		igtl::Sleep(100);
	}
	localMutex->Unlock();

	GLuint programID = LoadShaders("frag.frag", "vert.vert");

	// Vertex Shader source
	const std::string vs = isHolographic ?
		STRING
		(
			// holographic version

			uniform mat4 uModelMatrix;
	uniform mat4 uHolographicViewProjectionMatrix[2];
	attribute vec4 aPosition;
	attribute vec4 aColor;
	attribute float aRenderTargetArrayIndex;
	varying vec4 vColor;
	varying float vRenderTargetArrayIndex;
	void main()
	{
		int arrayIndex = int(aRenderTargetArrayIndex); // % 2; // TODO: integer modulus operation supported on ES 3.00 only
		gl_Position = uHolographicViewProjectionMatrix[arrayIndex] * uModelMatrix * aPosition;
		vColor = aColor;
		vRenderTargetArrayIndex = aRenderTargetArrayIndex;
	}
	) : STRING
	(
		uniform mat4 uModelMatrix;
	uniform mat4 uViewMatrix;
	uniform mat4 uProjMatrix;
	attribute vec4 aPosition;
	attribute vec4 aColor;
	varying vec4 vColor;
	void main()
	{
		gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
		vColor = aColor;
	}
	);

	// Fragment Shader source
	const std::string fs = isHolographic ? // TODO: this should not be necessary
		STRING
		(
			precision mediump float;
	varying vec4 vColor;
	varying float vRenderTargetArrayIndex; // TODO: this should not be necessary
	void main()
	{
		gl_FragColor = vColor;
		float index = vRenderTargetArrayIndex;
	}
	) : STRING
	(
		precision mediump float;
	varying vec4 vColor;
	void main()
	{
		gl_FragColor = vColor;
	}
	);

	// Set up the shader and its uniform/attribute locations.
	mProgram = CompileProgram(vs, fs);
	mPositionAttribLocation = glGetAttribLocation(mProgram, "aPosition");
	mColorAttribLocation = glGetAttribLocation(mProgram, "aColor");
	mRtvIndexAttribLocation = glGetAttribLocation(mProgram, "aRenderTargetArrayIndex");
	mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
	mViewUniformLocation = glGetUniformLocation(mProgram, "uViewMatrix");
	mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");

	// Then set up the cube geometry.
	/*float halfWidth = isHolographic ? 0.1f : 0.5f;
	GLfloat vertexPositions[] =
	{
	-halfWidth, -halfWidth, -halfWidth,
	-halfWidth, -halfWidth,  halfWidth,
	-halfWidth,  halfWidth, -halfWidth,
	-halfWidth,  halfWidth,  halfWidth,
	halfWidth, -halfWidth, -halfWidth,
	halfWidth, -halfWidth,  halfWidth,
	halfWidth,  halfWidth, -halfWidth,
	halfWidth,  halfWidth,  halfWidth,
	halfWidth *2, halfWidth *2, halfWidth*2,
	};


	GLfloat vertexColors[] =
	{
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	};
	*/
	float pos[3] = { 0 };
	std::list<igtlUint32> cell(3, 0);
	pointsArray->GetPoint(0, pos);
	int numPoints = pointsArray->GetNumberOfPoints();

	int numNormals = normArray->GetSize();

	int numPolys = polygonsArray->GetNumberOfCells();
	GLfloat* vertexColors = new GLfloat[3 * numPoints];
	//short* indices = new short[3 * numPolys];
	GLfloat centerPos[3] = { 0,0,0 };
	GLfloat* vertexPositions = new GLfloat[3 * numPoints];
	for (int i = 0; i < numPoints; i++) {

		pointsArray->GetPoint(i, pos);
		centerPos[0] += pos[0];
		centerPos[1] += pos[1];
		centerPos[2] += pos[2];
	}
	centerPos[0] /= numPoints;
	centerPos[1] /= numPoints;
	centerPos[2] /= numPoints;

	int iterate = 0;
	for (int i = 0; i < numPoints; i++) {

		pointsArray->GetPoint(i, pos);
		vertexPositions[3 * i] = pos[0] - centerPos[0];
		vertexPositions[3 * i + 1] = pos[1] - centerPos[1];
		vertexPositions[3 * i + 2] = pos[2] - centerPos[2] + 260; //the x plane of the object, increased by 200 to bring it closer
		
		
		// must now include surface normals for each vert, this will be multiplied by the collor 
		igtlFloat32 * norm0 = new igtlFloat32[2];

		normArray->GetNthData(iterate, norm0);
		iterate++;

		igtlFloat32 one = 1.0;
		
		// this will probably need to be moved out of loop, after poly loop
		vertexColors[3 * i] = 1 * .5;		//The color of the object by vertex
		vertexColors[3 * i + 1] = 1 * .5;
		vertexColors[3 * i + 2] = 1 * .5;
		vertexPositions[3 * i] /= 100.0;
		vertexPositions[3 * i+1] /= 100.0;
		vertexPositions[3 * i+2] /= 100.0;



	}


	//std::list<unsigned int>::iterator iter;
	std::list<igtlUint32>::iterator iter;
	short* indices = new short[3 *numPolys];
	for (int i = 0; i< numPolys; i++)
	{
		
		polygonsArray->GetCell(i, cell);
		int j = 0;
		for (iter = cell.begin(); iter != cell.end(); iter++)
		{
			indices[3 * i + j] = *iter;
			j++;
		}
	}

	//Let's just have some space to setup the lighting for this module

	/*
	const GLfloat ambientLight[] = { 0.05, 0.05, 0.05, 1.0 };
	GLint ambientLightUniformLocation = glGetUniformLocation(mProgram, ")


		*/

	//load normals in buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normArray)*numNormals * 3, normArray, GL_STATIC_DRAW);


	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	//float cosTheta = dot();
	




	//The surface normals will need to be calculated 

	int temp2 = sizeof(vertexPositions)*numPoints * 3;
	glGenBuffers(1, &mVertexPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions)*numPoints * 3, vertexPositions, GL_STATIC_DRAW);

	glGenBuffers(1, &mVertexColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors)*numPoints * 3, vertexColors, GL_STATIC_DRAW);
	/*short indicesreference[] =
	{
	0, 1, 2, // -x
	1, 3, 2,

	4, 6, 5, // +x
	5, 6, 7,

	0, 5, 1, // -y
	0, 4, 5,

	2, 7, 6, // +y
	2, 3, 7,

	0, 6, 4, // -z
	0, 2, 6,

	1, 7, 3, // +z
	1, 5, 7,
	};
	for (int i = 0; i < 36; i++)
		indices[i] = indicesreference[i];
	int temp = sizeof(indices);*/
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices)* numPolys * 3, indices, GL_STATIC_DRAW);

	float renderTargetArrayIndices[] = { 0.f, 1.f };
	glGenBuffers(1, &mRenderTargetArrayIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRenderTargetArrayIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(renderTargetArrayIndices), renderTargetArrayIndices, GL_STATIC_DRAW);

	mIsHolographic = isHolographic;
}

SimpleRenderer::~SimpleRenderer()
{
	if (mProgram != 0)
	{
		glDeleteProgram(mProgram);
		mProgram = 0;
	}

	if (mVertexPositionBuffer != 0)
	{
		glDeleteBuffers(1, &mVertexPositionBuffer);
		mVertexPositionBuffer = 0;
	}

	if (mVertexColorBuffer != 0)
	{
		glDeleteBuffers(1, &mVertexColorBuffer);
		mVertexColorBuffer = 0;
	}

	if (mIndexBuffer != 0)
	{
		glDeleteBuffers(1, &mIndexBuffer);
		mIndexBuffer = 0;
	}
}

void SimpleRenderer::Draw()
{
	glEnable(GL_DEPTH_TEST);

	//On HoloLens, it is important to clear to transparent.
	glClearColor(0.0f, 0.f, 0.f, 0.f);

	// On HoloLens, this will also update the camera buffers (constant and back).
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (mProgram == 0)
		return;

	glUseProgram(mProgram);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
	glEnableVertexAttribArray(mPositionAttribLocation);
	glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexColorBuffer);
	glEnableVertexAttribArray(mColorAttribLocation);
	glVertexAttribPointer(mColorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	MathHelper::Vec3 position = MathHelper::Vec3(0.f, 0.f, -2.f);
	MathHelper::Matrix4 modelMatrix = MathHelper::SimpleModelMatrix((float)mDrawCount / 2000000.0f, position);			//////////////////////////////
	glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, &(modelMatrix.m[0][0]));

	if (mIsHolographic)
	{
		// Load the render target array indices into an array.
		glBindBuffer(GL_ARRAY_BUFFER, mRenderTargetArrayIndices);
		glVertexAttribPointer(mRtvIndexAttribLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mRtvIndexAttribLocation);

		// Enable instancing.
		glVertexAttribDivisorANGLE(mRtvIndexAttribLocation, 1);

		// Draw 36 indices: six faces, two triangles per face, 3 indices per triangle
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glDrawElementsInstancedANGLE(GL_TRIANGLES, (polygonsArray->GetNumberOfCells() *3), GL_UNSIGNED_SHORT, 0, 2);
	}
	else
	{
		MathHelper::Matrix4 viewMatrix = MathHelper::SimpleViewMatrix();
		glUniformMatrix4fv(mViewUniformLocation, 1, GL_FALSE, &(viewMatrix.m[0][0]));

		MathHelper::Matrix4 projectionMatrix = MathHelper::SimpleProjectionMatrix(float(mWindowWidth) / float(mWindowHeight));
		glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, &(projectionMatrix.m[0][0]));

		// Draw 36 indices: six faces, two triangles per face, 3 indices per triangle
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glDrawElements(GL_TRIANGLES, (polygonsArray->GetNumberOfCells() * 3), GL_UNSIGNED_SHORT, 0);
	}

	mDrawCount += 1;
}

void SimpleRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
	if (!mIsHolographic)
	{
		glViewport(0, 0, width, height);

		mWindowWidth = width;
		mWindowHeight = height;
	}
}
