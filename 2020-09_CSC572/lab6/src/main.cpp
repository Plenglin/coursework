/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include <cmath>
#include "Program.h"
#include <time.h>
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

#define RASTERIZATION 15
#define TOTAL_CELLS (RASTERIZATION * RASTERIZATION * RASTERIZATION)

#define STARS_N 3000
#define CENTER_MASS 1e4
#define MIN_DIST 0.1
#define MAX_DIST 6
const float GRAV_CONST = 1e-4;

struct sphere {
    vec3 position;
    uint cell;
    vec3 velocity;
    float mass;
    vec3 acceleration;
    uint next;
    vec4 test;
};

struct cell {
    ivec3 barycenter_int;
    float mass;

    vec3 barycenter;
    uint head;

    uint array_list_start;
    uint count;
    uint _1;
    uint _2;
};

struct world_gpu_data {
    cell cells[TOTAL_CELLS];
    sphere objects[STARS_N];
};

float randf() {
    return (float)rand() / RAND_MAX;
}

class physics_world {
public:
    world_gpu_data data;
    GLuint objects_gpu;
    GLuint program;
    GLuint atomic_buf;
    GLuint object_block_index;
    GLuint uniform_dt, uniform_acc, uniform_centeredness;
    float centeredness = 1;

    void init_shader() {
        std::string shader_string = readFileAsString("../resources/phys_step.glsl");
        const char *cstr = shader_string.c_str();
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &cstr, nullptr);

        GLint rc;
        CHECKED_GL_CALL(glCompileShader(shader));
        CHECKED_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &rc));
        if (!rc) {      //error compiling the shader file
            GLSL::printShaderInfoLog(shader);
            std::cout << "Error compiling fragment shader " << std::endl;
            exit(1);
        }

        program = glCreateProgram();
        glAttachShader(program, shader);
        glLinkProgram(program);
        glUseProgram(program);

        object_block_index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "shader_data");
        glShaderStorageBlockBinding(program, object_block_index, 2);

        uniform_dt = glGetUniformLocation(program, "dt");
        uniform_acc = glGetUniformLocation(program, "G");
        uniform_centeredness = glGetUniformLocation(program, "centeredness");
    }

    void init_ssbo() {
        init_stars();
        glGenBuffers(1, &objects_gpu);
        upload();
    }

    void init_stars() {
        for (int i = 1; i < STARS_N; i++) {
            float angle = randf() * 2 * 3.1415;
            float radius = (MAX_DIST - MIN_DIST) * randf() + MIN_DIST;
            float c = cos(angle);
            float s = sin(angle);
            auto position = vec3(c, s, 0);
            position *= radius;
            position.z = randf() * 0.1;

            auto velocity = vec3(-s, c, 0);
            velocity *= 1 * sqrt(GRAV_CONST * CENTER_MASS / radius);

            data.objects[i].position = position;
            data.objects[i].velocity = velocity;
            data.objects[i].mass = 1;
        }
        data.objects[0].position = vec3(0, 0, 0);
        data.objects[0].velocity = vec3(0, 0, 0);
        data.objects[0].mass = CENTER_MASS;
    }

    void init_atomic() {
        glGenBuffers(1, &atomic_buf);
        // bind the buffer and define its initial storage capacity
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_buf);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_DYNAMIC_DRAW);
        // unbind the buffer
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    }

    void upload() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, objects_gpu);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(world_gpu_data), &data, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objects_gpu);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }

    world_gpu_data* mmap_ssbo(GLenum flags) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, objects_gpu);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, flags);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
        return (world_gpu_data*)p;
    }

    void munmap_ssbo() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, objects_gpu);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }

    void download() {
        auto* ref = mmap_ssbo(GL_READ_ONLY);
        for (int i = 0; i < STARS_N; i++) {
            data.objects[i] = ref->objects[i];
        }
        munmap_ssbo();
    }

    void write_points(vec3 *vert) const {
        for (int i = 0; i < STARS_N; i++) {
            vert[i] = data.objects[i].position;
        }
    }

    void step(float dt) {
        glShaderStorageBlockBinding(program, object_block_index, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objects_gpu);

        glUseProgram(program);
        glUniform1f(uniform_dt, dt);
        glUniform1f(uniform_centeredness, centeredness);
        glUniform1f(uniform_acc, GRAV_CONST);
        //activate atomic counter
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_buf);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_buf);

        glDispatchCompute(1, 1, 1);				//start compute shader
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    }
};

class Application : public EventCallbacks
{

public:
	physics_world world;

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, heightshader;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	GLuint VAO;
	GLuint VBO;

	// Data necessary to give our box to OpenGL
	GLuint MeshPosID, MeshTexID, IndexBufferIDBox;

	//texture data
	GLuint Texture;
	GLuint Texture2,HeightTex;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
		    world.init_stars();
            phys_to_vbo();
        }
	}

    void phys_to_vbo() const {
        vec3 vert[STARS_N];
        world.write_points(vert);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * STARS_N, vert);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
#define MESHSIZE 4
	void init_mesh()
	{

		//generate the VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);

		vec3 vert[STARS_N];
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * STARS_N, vert, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &MeshPosID);
		glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		vec3 vertices[MESHSIZE];
		
		vertices[0] = vec3(1.0, 0.0, 0.0);
		vertices[1] = vec3(0.0, 0.0, 0.0);
		vertices[2] = vec3(0.0, 0.0, 1.0);
		vertices[3] = vec3(1.0, 0.0, 1.0);
		

		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) *MESHSIZE, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//tex coords
		float t = 1. / 100;
		vec2 tex[MESHSIZE];
		tex[0] = vec2(1.0, 0.0);
		tex[1] = vec2(0,  0.0);
		tex[2] = vec2(0,  1.0);
		tex[3] = vec2(1.0, 1.0);

		glGenBuffers(1, &MeshTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort elements[6];
		int ind = 0;
		
		elements[0] = 0;
		elements[1] = 1;
		elements[2] = 2;
		elements[3] = 0;
		elements[4] = 2;
		elements[5] = 3;
				
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

    /*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//initialize the net mesh
		init_mesh();

		string resourceDirectory = "../resources" ;
		// Initialize mesh.
		shape = make_shared<Shape>();
		//shape->loadMesh(resourceDirectory + "/t800.obj");
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/grid.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/sky.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 3
		str = resourceDirectory + "/pluto.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &HeightTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, HeightTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);

		Tex1Location = glGetUniformLocation(heightshader->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(heightshader->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(heightshader->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		world.init_ssbo();
		world.init_atomic();
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
	
		prog->addAttribute("vertPos");


		// Initialize the GLSL program.
		heightshader = std::make_shared<Program>();
		heightshader->setVerbose(true);
		heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_frag.glsl");
		if (!heightshader->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		heightshader->addUniform("P");
		heightshader->addUniform("V");
		heightshader->addUniform("M");
		heightshader->addAttribute("vertPos");
		heightshader->addAttribute("vertTex");

		world.init_shader();
		mycam.pos = vec3(0, 0, -20);
	}

	void update(float dt) {
        // Update the physics world. Limit timesteps to ensure stability.
        float fixed_dt = std::min(dt, 0.02f);
        //world.step(fixed_dt / 5);
        world.step(0.02f);
        world.download();
        phys_to_vbo();
    }

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();
		update(frametime);

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		w += 1.0 * frametime;//rotation angle
		float trans = 0;// sin(t) * 2;
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = -3.1415926/2.0;
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

		M = mat4(1);// TransZ* RotateY* RotateX* S;

		// Draw the box using GLSL.
		prog->bind();

		V = mycam.process(frametime);
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VAO);	
		glDrawArrays(GL_POINTS, 0,STARS_N);

		prog->unbind();
		heightshader->bind();

	}

};
//******************************************************************************************
int main(int argc, char **argv)
{

	srand(time(0));

	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
