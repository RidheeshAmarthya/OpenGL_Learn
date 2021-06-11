#include"glew.h"
#include"glfw3.h"
#include<iostream>

static unsigned int ComplieShader(unsigned int type, const std::string& source) {

    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // Source must be alive when code is compliled
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error Handeling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE) {

        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char)); // VERY USEFUL STILL STORES IN THE STACK! // _malloca? alloca
        //dynamic array allocation ^
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile!" <<
            (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;

    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();
    unsigned int vs = ComplieShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = ComplieShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs); //glDetachShader() should be used here? it gives a minimal performance gain

    return program;
}

// OpenGL auto error idk how it works
void GLAPIENTRY MessageCallbacki(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    __debugbreak(); // ends execution, works only in VS
}

//Main loops
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Vsync


    if (glewInit() != GLEW_OK)
        std::cout << "Error" << std::endl;
    else
        std::cout << glGetString(GL_VERSION) << std::endl;



    //goto docs.gl for veryuseful OpenGL documentation!
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer; // Declaring the buffer (must be unsigned int) 
    glGenBuffers(1, &buffer); // generating buffer, stored in vram
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // informing it that this is a array buffer and binding it
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_DYNAMIC_DRAW); // * 3 check the docs!

    glEnableVertexAttribArray(0); // 0 is the index we want to enable it at???
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // Vertex Attribute 
    // 1. index of the buffer 2. 1,2,3,4 only it is the count so, 2 in this case as in 2d, 3. type of data, 4. normalised -? we can do it in cpu, 5. stride -byte offset so, in this case it is the size of float times 2 , 6. offset of the first component so, the starting location

    unsigned int ibo; // Declaring the buffer (must be unsigned int) 
    glGenBuffers(1, &ibo); // generating buffer, stored in vram
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // informing it that this is a array buffer and binding it
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW); // * 3 check the docs!

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 position;\n"
        "void main() \n"
        "{ gl_Position = vec4(position);"
        "}"
        ;

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color;\n"
        "uniform vec4 u_Color;\n"
        "void main() \n"
        "{ \n"
        "color = u_Color;\n"
        "}"
        ; //Comments dont work in here for some reason...


    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    //Uniform color 
    int location = glGetUniformLocation(shader, "u_Color");
    glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

    float r = 0.8;
    float increament = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) // every loop is a frame 
    {
        //Error handeling
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallbacki, 0);
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT); //Clear Screen OpenGl Function?? Needs opengl32.lib linked

        glUniform4f(location, r, 0.3f, 0.8f, 1.0f); // Uniform
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        r += increament; // Changin color!!
        if (r > 1)
            increament = -0.05f;
        else if (r < 0)
            increament = 0.05f;

        //glDrawArrays(GL_TRIANGLES, 0, 6); // Draws the currently bound buffer //6
        // When you dont have index buffer check docs!
        // ^ As it is a statemachine it already knows what is selected
        //glDrawElements() // With index buffer!


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}