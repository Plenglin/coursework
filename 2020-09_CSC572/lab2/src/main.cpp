/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/
//		showcase:
///		uniforms in CS
///		atomic counters
///		atomic operations
///		texture handling
///		workgroups

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include <time.h>
#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;

#define SORT_COUNT 11
struct ssbo_data {
    vec4 dataA[1024];
    ivec4 info[2];
};

float frand() {
	return (float)rand() / (float)RAND_MAX;
}

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}

class gpu_eosorter {
public:
    ssbo_data ssbo_cpu;
    GLuint ssbo_gpu;
    GLuint computeProgram;
    GLuint atomicsBuffer;
    const int invocations = (SORT_COUNT + 1) / 2 - 1;

    void init_atomic()
    {
        glGenBuffers(1, &atomicsBuffer);
        // bind the buffer and define its initial storage capacity
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_DYNAMIC_DRAW);
        // unbind the buffer
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    }
    void reset_atomic()
    {
        GLuint *userCounters;
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        // map the buffer, userCounters will point to the buffers data
        userCounters = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                                 0,
                                                 sizeof(GLuint),
                                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT
        );
        // set the memory to zeros, resetting the values in the buffer
        memset(userCounters, 0, sizeof(GLuint));
        // unmap the buffer
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }
    void read_atomic()
    {
        GLuint *userCounters;
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        // again we map the buffer to userCounters, but this time for read-only access
        userCounters = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                                 0,
                                                 sizeof(GLuint),
                                                 GL_MAP_READ_BIT
        );
        // copy the values to other variables because...
        cout << endl << *userCounters << endl;
        // ... as soon as we unmap the buffer
        // the pointer userCounters becomes invalid.
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }

    void init_program() {
        std::string ShaderString = readFileAsString("../resources/compute.glsl");
        const char *shader = ShaderString.c_str();
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

        GLuint block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
        glShaderStorageBlockBinding(computeProgram, block_index, 2);
    }

    void init_ssbo() {
        for (auto i = 0; i < 1024; i++) {
            ssbo_cpu.dataA[i] = vec4(-1, 0, 0, 0);
        }
        for (auto i = 0; i < SORT_COUNT; i++) {
            ssbo_cpu.dataA[i] = vec4(frand(), 0.0, 0.0, 0.0);
        }
        ssbo_cpu.info[0].x = SORT_COUNT;  // Correct count
        glGenBuffers(1, &ssbo_gpu);
    }

    void print() {
        for (auto i = 0; i < SORT_COUNT; i++)
            cout << i << ": " << ssbo_cpu.dataA[i].x << endl;
    }

    int run_cycle(int invocations, int offset) {

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gpu);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ssbo_data), &ssbo_cpu, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_gpu);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

        GLuint block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
        glShaderStorageBlockBinding(computeProgram, block_index, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_gpu);

        glUseProgram(computeProgram);
        //activate atomic counter
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicsBuffer);
        glDispatchCompute((GLuint)invocations, 1, 1);				//start compute shader
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gpu);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        memcpy(&ssbo_cpu, p, sizeof(ssbo_data));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        return ssbo_cpu.info[0].x;
    }

    void run() {
        int sorted_pairs = 0;
        run_cycle(invocations, 0);
    }

    void cleanup() {
    }
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	//texture data
	GLuint Texture;
	gpu_eosorter sort;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {}
	void mouseCallback(GLFWwindow *window, int button, int action, int mods) {}
	void resizeCallback(GLFWwindow *window, int in_width, int in_height) {}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
        sort.init_ssbo();
	}

	//General OGL initialization - set OGL state here
	void init()
	{
		GLSL::checkVersion();
		//load the compute shader
		sort.init_program();
	}

	void compute() {
        // print data before compute shader
        cout << "Dispatch counts: " << sort.invocations << endl;
        cout << endl << endl << "BUFFER BEFORE COMPUTE SHADER" << endl << endl;
        sort.print();

        sort.run();

        cout << endl << endl << "BUFFER AFTER COMPUTE SHADER" << endl << endl;
        sort.print();
    }
};
//******************************************************************************************
int main(int argc, char **argv)
{
		Application *application = new Application();
	srand(time(0));

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(32, 32, "Dummy", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGL();

	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);


	application->init();
	application->initGeom();

	application->sort.init_atomic();
	
	application->compute();

	application->sort.read_atomic();
	
	system("pause");
	return 0;
}
