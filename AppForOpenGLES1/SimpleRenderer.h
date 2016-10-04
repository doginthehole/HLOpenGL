#pragma once

#include "pch.h"
#include "MathHelper.h"

namespace AppForOpenGLES1
{
    class SimpleRenderer
    {
    public:
        SimpleRenderer(bool holographic);
        ~SimpleRenderer();
        void Draw();
        void UpdateWindowSize(GLsizei width, GLsizei height);

		//templated clamp function

/*
		double dot(double normal, double lightPos) {
			
			double result = abs(normal + lightPos);

			return result;
		}*/

    private:
        GLuint mProgram;
        GLsizei mWindowWidth;
        GLsizei mWindowHeight;

        GLint mPositionAttribLocation;
        GLint mColorAttribLocation;
		GLint mNormalAttribLocation;

        GLint mModelUniformLocation;
        GLint mViewUniformLocation;
        GLint mProjUniformLocation;
        GLint mRtvIndexAttribLocation;

        GLuint mVertexPositionBuffer;
        GLuint mVertexColorBuffer;
        GLuint mIndexBuffer;
        GLuint mRenderTargetArrayIndices;
		GLuint mNormalBuffer;

        int mDrawCount;
        bool mIsHolographic;

		//std::vector<Vector3> vertices;
		//std::vector<Vector4> colors;

    };
}