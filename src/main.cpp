#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include <GLM/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

#define HCF std::cin.get(); exit(-1)

double get_time(){
    using namespace std::chrono;
    milliseconds current_time_ms =
        duration_cast<milliseconds>( system_clock::now().time_since_epoch() );

    return current_time_ms.count() / 1000.;
}

std::string read_file(const char* filename){
    std::string file_contents;
    std::ifstream file_stream(filename, std::ios::in);

    if (file_stream.is_open()){
        std::stringstream file_ss;
        file_ss << file_stream.rdbuf();

        file_contents = file_ss.str();
        file_stream.close();

        return file_contents;
    }
    else{
        std::cerr << "Could not open file: " << filename << std::endl;
        HCF;
    }
}

bool compile_shader(unsigned int shader_id, const char* shader_filepath){
    // Load shader source code
    std::string shader_source = read_file(shader_filepath);
    const char* shader_source_pointer = shader_source.c_str();

    // Compile the shader
    glShaderSource(shader_id, 1, &shader_source_pointer, NULL);
    glCompileShader(shader_id);

    GLint compile_result = GL_FALSE;
    int info_log_length;

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

    if (compile_result == GL_TRUE){
        return true;
    }
    else{
        std::vector<char> shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(shader_id, info_log_length, nullptr, &shader_error_message[0]);
        printf("Could not compile shader %s. Error: %s\n", shader_filepath, &shader_error_message[0]);
        return false;
    }
}

bool link_shader(unsigned int vshad_id, unsigned int fshad_id, unsigned int program_id){
    glLinkProgram(program_id);

    GLint link_result = GL_FALSE;
    int info_log_length;

    glGetProgramiv(program_id, GL_LINK_STATUS, &link_result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

    if (link_result == GL_TRUE){
        return true;
    }
    else{
        std::vector<char> program_error_message(info_log_length + 1);
        glGetProgramInfoLog(program_id, info_log_length, nullptr, &program_error_message[0]);
        printf("Could not link shader program. Error: %s\n", &program_error_message[0]);
        return false;
    }
}


unsigned int create_shader_program(const char* vshad_filepath, const char* fshad_filepath){
    unsigned int vshad_id = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fshad_id = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int program_id = glCreateProgram();

    if ( !compile_shader(vshad_id, vshad_filepath) ){
        std::cerr << "Could not create shader program: Vertex shader compilation unsuccessfull.\n" << std::endl;
        HCF;
    }

    if ( !compile_shader(fshad_id, fshad_filepath) ){
        std::cerr << "Could not create shader program: Fragment shader compilation unsuccessful.\n" << std::endl;
        HCF;
    }

    glAttachShader(program_id, vshad_id);
    glAttachShader(program_id, fshad_id);

    if ( !link_shader(vshad_id, fshad_id, program_id) ){
        std::cerr << "Could not create shader program: Linking failed!" << std::endl;
        HCF;
    }

    glDetachShader(program_id, vshad_id);
    glDetachShader(program_id, fshad_id);

    glDeleteShader(vshad_id);
    glDeleteShader(fshad_id);

    return program_id;
}

static const float cube_vertex_pos_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

static const float cube_vertex_col_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

double vertex_data[] = {
    -.8, -.8,
     .8, -.8,
    0.0,  .8
};

double t;
const double cam_distance = 6.;
const unsigned int window_width = 800;
const unsigned int window_height = 600;
const float aspect_ratio = (float)window_width/((float)window_height);


const glm::mat4 model_matrix = glm::mat4(1.0f);

const glm::vec3 camera_focus       = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 camera_orientation = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_position;

glm::mat4 view_matrix;

const glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);

//glm::mat4 most_valuable_player = perspective_matrix * view_matrix * model_matrix;
glm::mat4 most_valuable_player;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    GLenum glew_init_error = glewInit();

    if (glew_init_error != GLEW_OK){
        std::cerr << "Failed to initialize GLEW. glewInit() returned error code "
            << glew_init_error << std::endl;
        return -1;
    }

    std::cout << "Using GL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Using GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    unsigned int vertex_pos_buffer;
    unsigned int vertex_col_buffer;
    glGenBuffers(1, &vertex_pos_buffer);
    glGenBuffers(1, &vertex_col_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_buffer);
    //glBufferData(GL_ARRAY_BUFFER, 3*2*sizeof(double), &vertex_data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 3*12*3*sizeof(float), &cube_vertex_pos_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_col_buffer);
    glBufferData(GL_ARRAY_BUFFER, 3*12*3*sizeof(float), &cube_vertex_pos_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);



    unsigned int rampant_shader = create_shader_program("res/shd/vertex_basic.shd", "res/shd/fragment_basic.shd");

    GLuint mvp_location = glGetUniformLocation(rampant_shader, "mvp");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)){
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        t = get_time();

        camera_position = glm::vec3(cam_distance*std::sin(t), 1.5, cam_distance*std::cos(t));
        view_matrix = glm::lookAt(camera_position, camera_focus, camera_orientation);
        most_valuable_player = projection_matrix * view_matrix * model_matrix;

        glUseProgram(rampant_shader);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &most_valuable_player[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 3*12);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
