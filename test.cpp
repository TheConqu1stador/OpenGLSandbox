#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
    std::string load_shader(const char* path) { 
       
        std::ifstream shader_file;
        std::string out_string;

        shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            shader_file.open(path);
            std::stringstream shader_stream;

            shader_stream << shader_file.rdbuf();

            shader_file.close();

            out_string = shader_stream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << path << std::endl;
        }

        return out_string;
    }
    
    virtual int compile_shader() = 0;
};

class Fragment_shader : Shader {
public:
    void set_shader_source(const char* path) {
		fragment_shader_source = load_shader(path);
	}

    int compile_shader() {
		
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* temp_source = fragment_shader_source.c_str();
		glShaderSource(fragment_shader, 1, &temp_source, nullptr);
		glCompileShader(fragment_shader);

		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success_fragment);

		if (!success_fragment) {
			glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log_fragment);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log_fragment << std::endl;
		}
		
		return 0;
    }

    int get_fragment_shader() {
        return fragment_shader;
    }
    
private:
	std::string fragment_shader_source;
    int fragment_shader;
    int success_fragment;
    char info_log_fragment[512];
};

class Vertex_shader : Shader {
public:
    
    void set_shader_source(const char* path) {
		vertex_shader_source = load_shader(path);
	}

    int compile_shader() {
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        const char* temp_source = vertex_shader_source.c_str();
        glShaderSource(vertex_shader, 1, &temp_source, nullptr);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success_vertex);
        
        if (!success_vertex) {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log_vertex);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log_vertex << std::endl;
            return -1;
        }

        return 0;
    }

    int get_vertex_shader() {
        return vertex_shader;
    }
    
private:
	std::string vertex_shader_source;
    int vertex_shader;
    int success_vertex;
    char info_log_vertex[512];
};

class Shader_program {
public:
    
    void create_program() {
		shader_program = glCreateProgram();
	}

    void attach_shader(int shader) {
        glAttachShader(shader_program, shader);
    }

    void link_program() {
        glLinkProgram(shader_program);

        glGetProgramiv(shader_program, GL_LINK_STATUS, &success_program);

        if (!success_program) {
            glGetProgramInfoLog(shader_program, 512, nullptr, info_log_program);
            std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << info_log_program << std::endl;
        }
    }


    int get_shader_program() {
        return shader_program;
    }
    
private:
    int shader_program;
    int success_program;
    char info_log_program[512];
};

class GLInit {
public:
    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {};
    void process_input(GLFWwindow* window0) {};

    void start() {
        glfwInit();
    };

    void set_version(int major, int minor) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    };

    void set_profile() {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }; //core

    int make_new_window(int width, int height, char* name) {
        current_window = glfwCreateWindow(width, height, name, NULL, NULL);

        if (current_window == nullptr) {
            std::cout << "ERROR::WINDOW::MAKE\n";
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(current_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "ERROR::LOADGLLOADER::FAILED" << std::endl;
            return -1;
        }

        glViewport(0, 0, width, height);

        return 0;
    }

    void close_current_window() {
        glfwSetWindowShouldClose(current_window, true);
    }

    //a-z A-z 0-9
    void close_current_window(char key) {
        if (glfwGetKey(current_window, (int)key) == GLFW_PRESS)
          glfwSetWindowShouldClose(current_window, true);
    }

    GLFWwindow* get_current_window() {
        return current_window;
    }

private:
    GLFWwindow* current_window;
};


int main()
{
    GLInit *init = new GLInit;
    Vertex_shader *vertex_shader = new Vertex_shader;
    Fragment_shader *fragment_shader = new Fragment_shader;
    Shader_program *shader_program = new Shader_program;

    init->start();
    init->set_version(4, 0);
    init->set_profile();
    init->make_new_window(800, 600, "sooqa");

    vertex_shader->set_shader_source("vertex.vs");
    vertex_shader->compile_shader();
    
    fragment_shader->set_shader_source("fragment.fs");
    fragment_shader->compile_shader();

    shader_program->create_program();
    shader_program->attach_shader(vertex_shader->get_vertex_shader());
    shader_program->attach_shader(fragment_shader->get_fragment_shader());
    shader_program->link_program();
    
    float vertices[] = {
        -0.3f, -0.9f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.2f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f,  0.3f, 0.0f, 0.0f, 0.0f, 1.0f
    };


    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3))  ;
    glEnableVertexAttribArray(1);


    while (!glfwWindowShouldClose(init->get_current_window())) {
        init->close_current_window('F');

        glClearColor(0, 0, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program->get_shader_program());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(init->get_current_window());
        glfwPollEvents();
    }

    return 0;
}



