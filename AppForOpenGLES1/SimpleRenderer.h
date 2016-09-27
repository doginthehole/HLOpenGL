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
		template < class X >
		X template clamp(X input, X min, X max) {
			if (input < min)
				return min;
			else if (input > max)
				return max;
			return input;
		}

		double dot(double normal, double lightPos) {
			
			double result = abs(normal + lightPos);

			return result;
		}

    private:
        GLuint mProgram;
        GLsizei mWindowWidth;
        GLsizei mWindowHeight;

        GLint mPositionAttribLocation;
        GLint mColorAttribLocation;

        GLint mModelUniformLocation;
        GLint mViewUniformLocation;
        GLint mProjUniformLocation;
        GLint mRtvIndexAttribLocation;

        GLuint mVertexPositionBuffer;
        GLuint mVertexColorBuffer;
        GLuint mIndexBuffer;
        GLuint mRenderTargetArrayIndices;

        int mDrawCount;
        bool mIsHolographic;
    };
}