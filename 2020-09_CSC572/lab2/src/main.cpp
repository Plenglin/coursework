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

#define SORT_COUNT 12
struct ssbo_data {
    vec4 dataA[SORT_COUNT];
    ivec4 info[1];
};
float frand()
	{
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
    GLuint ssbo_GPU_id;
    GLuint computeProgram;
    GLuint atomicsBuffer;
    const int oddSortCount = (SORT_COUNT + 1) / 2 - 1;
    const int evenSortCount = SORT_COUNT / 2;

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

        GLuint block_index = 0;
        block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
        GLuint ssbo_binding_point_index = 2;
        glShaderStorageBlockBinding(computeProgram, block_index, ssbo_binding_point_index);
    }

    void init_ssbo() {
        for (int ii = 0; ii < SORT_COUNT; ii++) {
            ssbo_cpu.dataA[ii] = vec4(frand(), 0.0, 0.0, 0.0);
        }
        glGenBuffers(1, &ssbo_GPU_id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_GPU_id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ssbo_data), &ssbo_cpu, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_GPU_id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }

    void print() {
        for (int i = 0; i < SORT_COUNT; i++)
            cout << "dataA: " << ssbo_cpu.dataA[i].x << endl;
    }

    void run() {

        GLuint block_index = 0;
        block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
        GLuint ssbo_binding_point_index = 0;
        glShaderStorageBlockBinding(computeProgram, block_index, ssbo_binding_point_index);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_GPU_id);
        glUseProgram(computeProgram);
        //activate atomic counter
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicsBuffer);

        glDispatchCompute((GLuint)evenSortCount, (GLuint)1, 1);				//start compute shader
        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

        //copy data back to CPU MEM

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_GPU_id);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        int siz = sizeof(ssbo_data);
        memcpy(&ssbo_cpu, p, siz);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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
        cout << "Dispatch counts: " << sort.evenSortCount << "e " << sort.oddSortCount << "o" << endl;
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
