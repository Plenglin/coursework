/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

#define RESX 800
#define RESY 450


shared_ptr<Shape> shape;
std::vector<float> bufInstance(RESX * RESY * 4);
std::vector<unsigned char> buffer(RESX * RESY * 4);
std::vector<unsigned char> buffer2(RESX * RESY * 4);

GLuint computeProgram, computeProgramVort, computeProgramPaint, computeProgramSurface, compute_program_uniform_center, compute_program_uniform_color, compute_program_uniform_radius;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}

template <int button>
class DragHandler {
public:
    ivec2 position;
    ivec2 delta;

    bool update(GLFWwindow *window) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        auto current_pos = ivec2(x, y);
        bool pressed = glfwGetMouseButton(window, button);

        delta = pressed ? current_pos - position : ivec2(0, 0);
        position = current_pos;

        return pressed;
    }
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> postproc;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID, VertexArrayIDScreen;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexBufferTexScreen, VertexBufferIDScreen,VertexNormDBox, VertexTexBox, IndexBufferIDBox, InstanceBuffer;

	//framebufferstuff
	GLuint fb, depth_fb, FBOtex;
	//texture data
	GLuint Texture, Texture2, wall, surface1, surface2;
    GLuint CS_tex_A, CS_tex_B, CS_wall_tex;

    DragHandler<GLFW_MOUSE_BUTTON_1> left_drag;
    DragHandler<GLFW_MOUSE_BUTTON_2> right_drag;

	int tex_w, tex_h;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS) {
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
	    string resourceDirectory = "../resources";


		//screen plane
		glGenVertexArrays(1, &VertexArrayIDScreen);
		glBindVertexArray(VertexArrayIDScreen);
		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDScreen);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDScreen);
		vec3 vertices[6];
		vertices[0] = vec3(-1,-1,0);
		vertices[1] = vec3(1, -1, 0);
		vertices[2] = vec3(1, 1, 0);
		vertices[3] = vec3(-1, -1, 0);
		vertices[4] = vec3(1, 1, 0);
		vertices[5] = vec3(-1, 1, 0);
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3), vertices, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferTexScreen);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTexScreen);
		vec2 texscreen[6];
		texscreen[0] = vec2(0, 0);
		texscreen[1] = vec2(1, 0);
		texscreen[2] = vec2(1, 1);
		texscreen[3] = vec2(0, 0);
		texscreen[4] = vec2(1, 1);
		texscreen[5] = vec2(0, 1);
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), texscreen, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(1);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindVertexArray(0);

		int width, height, channels;
		char filepath[1000];

		//texture 1

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location;

		Tex1Location = glGetUniformLocation(postproc->pid, "tex");//tex, tex2... sampler in the fragment shader
		glUseProgram(postproc->pid);
		glUniform1i(Tex1Location, 0);


		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		//RGBA8 2D texture, 24 bit depth texture, 256x256
		//-------------------------
		//Does the GPU support current FBO configuration?
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			cout << "status framebuffer: good";
			break;
		default:
			cout << "status framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!";
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//load input image
        unsigned char* pic_data = stbi_load("../resources/testflow2.png", &width, &height, &channels, 4); //store the input data on the CPU memory and get the address
        float* temp;
        temp = (float*)malloc(RESX * RESY * 4 * sizeof(float));
        for (int i = 0; i < RESX * RESY * 4; i++)
            temp[i] = (float)pic_data[i] / 256;

        //make a texture (buffer) on the GPU to store the input image
		tex_w = width, tex_h = height;		//size
		glGenTextures(1, &CS_tex_A);		//Generate texture and store context number
		glActiveTexture(GL_TEXTURE0);		//since we have 2 textures in this program, we need to associate the input texture with "0" meaning first texture
		glBindTexture(GL_TEXTURE_2D, CS_tex_A);	//highlight input texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//texture sampler parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//texture sampler parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//texture sampler parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//texture sampler parameter
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, temp);	//copy image data to texture
		glBindImageTexture(0, CS_tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);	//enable texture in shader

        stbi_image_free(temp);

        //make a texture (buffer) on the GPU to store the output image
        glGenTextures(1, &CS_tex_B);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, CS_tex_B);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, nullptr);
        glBindImageTexture(1, CS_tex_B, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        pic_data = stbi_load("../resources/wall.png", &width, &height, &channels, 4); //store the input data on the CPU memory and get the address
        temp = (float*)malloc(RESX * RESY * 4 * sizeof(float));
        for (int i = 0; i < RESX * RESY * 4; i++)
            temp[i] = (float)pic_data[i] / 256;

        //make a texture (buffer) on the GPU to store the output image
        glGenTextures(1, &CS_wall_tex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, CS_wall_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, temp);
        glBindImageTexture(2, CS_wall_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        stbi_image_free(temp);

        pic_data = stbi_load("../resources/surface.png", &width, &height, &channels, 4); //store the input data on the CPU memory and get the address
        temp = (float*)malloc(RESX * RESY * 4 * sizeof(float));
        for (int i = 0; i < RESX * RESY * 4; i++)
            temp[i] = (float)pic_data[i] / 256;

        //make a texture (buffer) on the GPU to store the output image
        glGenTextures(1, &surface1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, surface1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, temp);
        glBindImageTexture(3, surface1, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glGenTextures(1, &surface2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, surface2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, nullptr);

        stbi_image_free(temp);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.

		//program for the postprocessing
		postproc = std::make_shared<Program>();
		postproc->setVerbose(true);
		postproc->setShaderNames(resourceDirectory + "/postproc_vertex.glsl", resourceDirectory + "/postproc_fragment.glsl");
		if (!postproc->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		postproc->addAttribute("vertPos");
		postproc->addAttribute("vertTex");

        //load the compute shader
        std::string ShaderString = readFileAsString("../resources/compute.glsl");
        const char* shader = ShaderString.c_str();
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeShader, 1, &shader, nullptr);
        GLint rc;
        CHECKED_GL_CALL(glCompileShader(computeShader));
        CHECKED_GL_CALL(glGetShaderiv(computeShader, GL_COMPILE_STATUS, &rc));
        if (!rc)	//error compiling the shader file
        {
            GLSL::printShaderInfoLog(computeShader);
            std::cout << "Error compiling fragment shader " << std::endl;
            exit(1);
        }
        computeProgram = glCreateProgram();
        glAttachShader(computeProgram, computeShader);
        glLinkProgram(computeProgram);
        glUseProgram(computeProgram);

        //load the vorticity shader
        std::string VortString = readFileAsString("../resources/compute_vort.glsl");
        const char* shader_vort = VortString.c_str();
        GLuint computeVort = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeVort, 1, &shader_vort, nullptr);
        GLint rcv;
        CHECKED_GL_CALL(glCompileShader(computeVort));
        CHECKED_GL_CALL(glGetShaderiv(computeVort, GL_COMPILE_STATUS, &rcv));
        if (!rcv)	//error compiling the shader file
        {
            GLSL::printShaderInfoLog(computeVort);
            std::cout << "Error compiling vorticity shader " << std::endl;
            exit(1);
        }
        computeProgramVort = glCreateProgram();
        glAttachShader(computeProgramVort, computeVort);
        glLinkProgram(computeProgramVort);
        glUseProgram(computeProgramVort);

        //load the surface shader
        std::string SurfaceString = readFileAsString("../resources/compute_surface.glsl");
        const char* shader_surface = SurfaceString.c_str();
        GLuint computeSurface = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeSurface, 1, &shader_surface, nullptr);
        GLint rcs;
        CHECKED_GL_CALL(glCompileShader(computeSurface));
        CHECKED_GL_CALL(glGetShaderiv(computeSurface, GL_COMPILE_STATUS, &rcs));
        if (!rcs)	//error compiling the shader file
        {
            GLSL::printShaderInfoLog(computeSurface);
            std::cout << "Error compiling surface shader " << std::endl;
            exit(1);
        }
        computeProgramSurface = glCreateProgram();
        glAttachShader(computeProgramSurface, computeSurface);
        glLinkProgram(computeProgramSurface);
        glUseProgram(computeProgramSurface);

        // paint shader
        std::string paint_string = readFileAsString("../resources/paint.glsl");
        const char* shader_paint = paint_string.c_str();
        GLuint compute_paint = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute_paint, 1, &shader_paint, nullptr);
        GLint rcp;
        CHECKED_GL_CALL(glCompileShader(compute_paint));
        CHECKED_GL_CALL(glGetShaderiv(compute_paint, GL_COMPILE_STATUS, &rcp));
        if (!rcp)	//error compiling the shader file
        {
            GLSL::printShaderInfoLog(compute_paint);
            std::cout << "Error compiling painting shader " << std::endl;
            exit(1);
        }
        computeProgramPaint = glCreateProgram();
        glAttachShader(computeProgramPaint, compute_paint);
        glLinkProgram(computeProgramPaint);
        glUseProgram(computeProgramPaint);
        compute_program_uniform_radius = glGetUniformLocation(computeProgramPaint, "radius");
        compute_program_uniform_center = glGetUniformLocation(computeProgramPaint, "center");
        compute_program_uniform_color = glGetUniformLocation(computeProgramPaint, "color");
	}

	inline void swap_buffers() {
        GLuint tmp = CS_tex_A;
        CS_tex_A = CS_tex_B;
        CS_tex_B = tmp;
	}

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/

	int compute(int printframes) {
        left_drag.update(windowManager->getHandle());
        right_drag.update(windowManager->getHandle());

        if (left_drag.delta.x != 0 && left_drag.delta.y != 0) {
            const int radius = 25;
            ivec2 delta = left_drag.delta;
            delta.y = -delta.y;

            const vec4 color = vec4((normalize(vec2(delta)) + 1.0f) / 2.0f, 1.0f, 1.0f);

            int w, h;
            glfwGetFramebufferSize(windowManager->getHandle(), &w, &h);
            ivec2 pos = left_drag.position;
            pos.y = h - pos.y;
            pos = pos * ivec2(tex_w, tex_h) / ivec2(w, h);

            glUseProgram(computeProgramPaint);
            glBindImageTexture(0, CS_tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            glUniform1i(compute_program_uniform_radius, radius);
            glUniform2iv(compute_program_uniform_center, 1, &pos[0]);
            glUniform4fv(compute_program_uniform_color, 1, &color[0]);
            glDispatchCompute((GLuint)(radius * 2), (GLuint)(radius * 2), 1);

            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        for (int i = 0; i < 1; i++) {
            glUseProgram(computeProgramSurface);
            glBindImageTexture(1, surface2, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            glBindImageTexture(3, surface1, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            glUseProgram(computeProgramSurface);
            glBindImageTexture(1, surface1, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            glBindImageTexture(3, surface2, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 1; j++) {
                glUseProgram(computeProgram);
                glBindImageTexture(0, CS_tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                glBindImageTexture(1, CS_tex_B, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                glDispatchCompute((GLuint) tex_w, (GLuint) tex_h, 1);
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
                swap_buffers();
            }

            for (int j = 0; j < 1; j++) {
                glUseProgram(computeProgramVort);
                glBindImageTexture(0, CS_tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                glBindImageTexture(1, CS_tex_B, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
                swap_buffers();
            }
        }

        glBindImageTexture(0, CS_tex_A, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        return 0;
	}
	//*****************************************************************************************
	void render(int texnum)
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
		// Clear framebuffer.
		glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postproc->bind();
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, surface1);

		glBindVertexArray(VertexArrayIDScreen);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		postproc->unbind();
	}
};
//******************************************************************************************
int main(int argc, char **argv)
{
	//initialize Open GL
	glfwInit();
	//make a window
	GLFWwindow* window = glfwCreateWindow(512, 512, "Dummy", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	//initialize Open GL Loader function
	gladLoadGL();

	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application* application = new Application();
	/* your main will always include a similar set up to establish your window
	and GL context, etc. */
	WindowManager* windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
		// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	glUseProgram(computeProgram);
	glUseProgram(computeProgramVort);

	// Loop until the user closes the window.
	double timef = 0;
	int printframes = 0;
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{

		int ret = application->compute(printframes++);

		application->render(ret);

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
		//timef = 1./get_last_elapsed_time();
		//printf("%f\n", timef);
	}

	/*int width = RESX, height = RESY;
	stbi_write_png("output.png", width, height, 4, buffer.data(), 0);
	stbi_write_png("output2.png", width, height, 4, buffer2.data(), 0);
	*/
	// Quit program.
	windowManager->shutdown();
	return 0;
}
