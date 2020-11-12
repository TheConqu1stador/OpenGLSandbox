#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
    Shader(int external_shader_type) : shader_type{ external_shader_type }
    {

    }

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

    void set_shader_source(const char* path) {
        shader_source = load_shader(path);
    }

    int compile_shader() {
        shader = glCreateShader(shader_type);
        
        const char* temp_source = shader_source.c_str();
        glShaderSource(shader, 1, &temp_source, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success_fragment);

        if (!success_fragment) {
            glGetShaderInfoLog(shader, 512, nullptr, info_log_fragment);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log_fragment << std::endl;
            return -1;
        }

        return 0;
    }

    int get_shader() {
        return shader;
    }

private:
    std::string shader_source;
    int shader;
    int success_fragment;
    char info_log_fragment[512];
    int shader_type;
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

class Figure {
public:
    Figure(char external_type_name[], std::vector< float> external_vertices) 
        : type_name{ external_type_name }, vertices { external_vertices }
    {

    }

    void set_buffers() {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, get_size(), get_vertices(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
    }

    void draw_figure() {
        glBindVertexArray(VAO);

        if (type_name == "TRIANGLE") {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        else if (type_name == "RECTANGLE") {
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawArrays(GL_TRIANGLES, 1, 3);
        }
    }

    float* get_vertices() {
        return &vertices[0];
    }

    int get_size() {
        return vertices.size() * sizeof(vertices[0]);
    }

private:
    std::vector< float> vertices;
    char *type_name;
    unsigned int VBO, VAO;
};

int main()
{
    GLInit *init = new GLInit;
    Shader *vertex_shader = new Shader(GL_VERTEX_SHADER);
    Shader *fragment_shader = new Shader(GL_FRAGMENT_SHADER);
    Shader_program *shader_program = new Shader_program;

    init->start();
    init->set_version(4, 0);
    init->set_profile();
    init->make_new_window(800, 600, "test");

    vertex_shader->set_shader_source("vertex.vs");
    vertex_shader->compile_shader();

    fragment_shader->set_shader_source("fragment.fs");
    fragment_shader->compile_shader();

    shader_program->create_program();
    shader_program->attach_shader(vertex_shader->get_shader());
    shader_program->attach_shader(fragment_shader->get_shader());
    shader_program->link_program();

    std::vector< float> vertices_rectangle = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.3f, 0.0f, 1.0f, 0.0f, 1.0f,
    };

    std::vector< float> vertices_triangle = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    Figure *triangle = new Figure("TRIANGLE", vertices_triangle);
    Figure *rectangle = new Figure("RECTANGLE", vertices_rectangle);

    triangle->set_buffers();
    rectangle->set_buffers();

    while (!glfwWindowShouldClose(init->get_current_window())) {
        init->close_current_window('F');

        glClearColor(0, 0, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program->get_shader_program());

        triangle->draw_figure();
        rectangle->draw_figure();

        glfwSwapBuffers(init->get_current_window());
        glfwPollEvents();
    }

    return 0;
}



