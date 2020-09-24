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

// Note: One workgroup works on a contiguous chunk of 2 * WORKGROUP_SIZE items.
#define WORKGROUP_SIZE 8

struct sort_data {
    ivec4 size;
    vec4 dataA[4096];
};

// Stored separate from the data so there's less memory to copy.
struct global_info {
    // true if sorted, false otherwise
    int global_sorted;
    int even;
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
    sort_data ssbo_cpu;
    global_info global_cpu;
    GLuint ssbo_gpu;
    GLuint global_gpu;
    GLuint computeProgram;
    GLuint atomicsBuffer;
    GLuint shader_data_block_index;
    GLuint global_info_block_index;

    const int sort_count;

    const int odd_groups;
    const int even_groups;

    gpu_eosorter(int size) :
            sort_count(size),
            odd_groups((size + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE - 1),  // ceil(size / workgroupsize) - 1
            even_groups(size / WORKGROUP_SIZE)  // floor(size / workgroupsize)
    {

    }

    void init_atomic() {
        glGenBuffers(1, &atomicsBuffer);
        // bind the buffer and define its initial storage capacity
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_DYNAMIC_DRAW);
        // unbind the buffer
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    }

    void reset_atomic() {
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

    void read_atomic() {
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

        shader_data_block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");
        global_info_block_index = glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "global_info");

        glShaderStorageBlockBinding(computeProgram, shader_data_block_index, 2);
        glShaderStorageBlockBinding(computeProgram, global_info_block_index, 3);
    }

    void init_ssbo() {
        for (auto i = 0; i < sort_count; i++) {
            ssbo_cpu.dataA[i] = vec4(-1, 0, 0, 0);
        }
        for (auto i = 0; i < sort_count; i++) {
            ssbo_cpu.dataA[i] = vec4(frand(), 0.0, 0.0, 0.0);
        }
        ssbo_cpu.size.x = sort_count;  // Correct count
        glGenBuffers(1, &ssbo_gpu);
        glGenBuffers(1, &global_gpu);
    }

    void print() {
        for (auto i = 0; i < sort_count; i++)
            cout << i << ": " << ssbo_cpu.dataA[i].x << endl;
    }

    void upload_ssbo() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gpu);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(sort_data), &ssbo_cpu, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_gpu);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }

    void upload_global() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, global_gpu);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(global_info), &global_gpu, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, global_gpu);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }

    void download_ssbo() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gpu);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        memcpy(&ssbo_cpu, p, sizeof(sort_data));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    void download_global() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, global_gpu);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        memcpy(&global_cpu, p, sizeof(global_info));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    int run_cycle(bool even) {
        auto invocations = even ? even_groups : odd_groups;
        global_cpu.even = even;
        global_cpu.global_sorted = 1;
        upload_global();

        glShaderStorageBlockBinding(computeProgram, shader_data_block_index, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_gpu);

        glShaderStorageBlockBinding(computeProgram, global_info_block_index, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, global_gpu);

        glUseProgram(computeProgram);
        //activate atomic counter
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicsBuffer);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicsBuffer);

        glDispatchCompute((GLuint)invocations, 1, 1);				//start compute shader
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

        download_global();
        return global_cpu.global_sorted;
    }

    // Run a cascaded odd-even sort. The outer cascade is managed by the CPU, while the inner cascade is run on the GPU.
    void run() {
        upload_ssbo();
        int result = run_cycle(true);
        download_ssbo();
    }

    void cleanup() {
    }
};


class Application : public EventCallbacks {

public:

	WindowManager * windowManager = nullptr;
	//texture data
	GLuint Texture;
	gpu_eosorter sort = gpu_eosorter(64);

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
        cout << "Dispatch counts: " << sort.even_groups << endl;
        cout << endl << endl << "BUFFER BEFORE COMPUTE SHADER" << endl << endl;
        sort.print();

        auto start = clock();
        sort.run();
        auto duration = clock() - start;

        cout << endl << endl << "BUFFER AFTER COMPUTE SHADER" << endl << endl;
        sort.print();
        cout << "Completed in " << duration << endl;
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
