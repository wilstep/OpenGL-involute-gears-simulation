// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL V3


#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <memory>
#include <vector>
#include "matrix.h"
#include "gear.h"

static const GLuint Na = 80;
static const GLuint Nb = 16;
static const GLfloat ytheta = -25.0f; // rotate around y axis

// Shader sources, 
const GLchar* vertexSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    out vec3 Normal;
    out vec3 FragPos;
    
    uniform mat4 rotate; 
    uniform mat4 perspective, rot2;
    mat4 product, product2;
    vec4 position;
    vec4 pos;
    
    void main()
    {    
        position = vec4(aPos, 1.0);
        product = rot2 * rotate;
        product2 = perspective * product;
        pos = product2 * position;
        gl_Position = pos; // vertex output position
        Normal = vec3(product * vec4(aNormal, 0.0)); 
        FragPos = vec3(product * vec4(aPos, 1.0));
    }
)glsl";

const GLchar* fragmentSource = R"glsl(
    #version 330 core
    uniform vec3 triangleColor;
    uniform vec3 lightPos;
    
    in vec3 Normal;
    in vec3 FragPos;
    out vec4 outColor;
    
    void main()
    {
        vec3 lightColor = vec3(0.9, 0.9, 0.9);
        float ambientStrength = 0.2;
        
        vec3 ambient = ambientStrength * lightColor;
        vec3 lightDir = normalize(lightPos - FragPos);
        //vec3 norm = vec3(normalize(Normal));
        vec3 norm = Normal;
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor; 
        vec3 result = (ambient + diffuse) * triangleColor;
        outColor = vec4(result, 1.0);
        //outColor = vec4(triangleColor, 1.0);
    }
)glsl";

// return a 4 * 4 rotation matrix for a rotation around k (z axis)
void rotate_z(GLfloat theta, std::vector<GLfloat> &R)
{
    GLfloat sinr, cosr;
    sinr = sin(theta);
    cosr = cos(theta);
    R = {
        cosr, -sinr, 0.0f, 0.0f,
        sinr, cosr, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f 
    };
}

// returns 1 if the GLEW setup worked
GLuint glewSetupCheck()
{
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    std::cout << vertexBuffer << std::endl;
    return vertexBuffer;
}

class SetUp
{
public:
    SetUp();
    ~SetUp();
    void setColor(GLfloat x, GLfloat y, GLfloat z) { glUniform3f(uniColor, x, y, z); }
    void rotate_a();
    void rotate_b();
    GLuint GetNInds_a(){ return Nind_a; }
    GLuint GetN1Inds_a(){ return Nind1_a; }
    GLuint GetNInds_b(){ return Nind_b; }
    GLuint GetN1Inds_b(){ return Nind1_b; }
private:
    void createBuff();
    GLuint Nind_a, Nind1_a, Nind_b, Nind1_b;
    GLuint vao, vbo, ebo;   
    GLuint shaderProgram;
    GLint uniColor;
    GLint uniRotate, uniPerspective, uniRot2;
    double theta_a = 0.0f, theta_b = 0.0f;
    matrix P;
};

SetUp::SetUp()
{
    // create buffers for verticies and indices
    createBuff();

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[1024];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }    
    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }    

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glUseProgram(shaderProgram);
    
    // Grab the uniform variables
    // Grab the uniform location for colour setting
    uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    setColor(1.0f, 0.0f, 0.0f);
    // and rotation
    uniRotate = glGetUniformLocation(shaderProgram, "rotate");
    uniPerspective = glGetUniformLocation(shaderProgram, "perspective");
    P.perspective(30.0f, 4.0f/3.0f, 0.1f, 180.0f);
    glUniformMatrix4fv(uniPerspective, 1, GL_TRUE, P.data());
    uniRot2 = glGetUniformLocation(shaderProgram, "rot2");
    // 2nd rotation
    P.rotate(ytheta, 0.0f, 1.0f, 0.0f); // 55.0f
    // shifts after rotation
    //P.putVdat(3, 3.0f); // 3, shift along x axis
    //P.putVdat(7, -4.0f); // 3, shift along y axis
    //P.putVdat(11, -25.0f); // 11, shift along z axis, -60
    P.putVdat(11, -55.0f); // 11, shift along z axis, -60
    glUniformMatrix4fv(uniRot2, 1, GL_TRUE, P.data());
    // set light position
    GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3f(uniLightPos, 50.0f, 400.0f, 150.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST); 
}

SetUp::~SetUp()
{
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void SetUp::createBuff()
{
    std::unique_ptr<gear> myGa = std::make_unique<gear>(Na);
    std::unique_ptr<gear> myGb = std::make_unique<gear>(Nb);
    
    Nind_a = myGa -> GetNInds();
    Nind1_a = myGa -> GetN1Inds();
    Nind_b = myGb -> GetNInds();
    Nind1_b = myGb -> GetN1Inds();
    myGa -> RotateVerts(90.0f);
    myGb -> RotateVerts(-90.0f);
    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo); // and an element buffer object
    
    // vertex data
    std::vector<GLfloat> vertices, vbuff;
    vertices = myGa -> GetVerts();
    vbuff = myGb -> GetVerts();
    vertices.insert(vertices.end(), vbuff.begin(), vbuff.end());
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
   
    // index data
    std::vector<GLuint> indices, ibuff;
    indices = myGa -> GetInds();
    ibuff = myGb -> GetInds();
    GLuint nverts = myGa -> GetNverts();
    for(auto &a: ibuff) a += nverts;
    indices.insert(indices.end(), ibuff.begin(), ibuff.end());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // element buffer object
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    // Specify the layout of the vertex data
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

void SetUp::rotate_a()
{
    static const GLfloat delx = (GLfloat) Nb * 0.5f + (GLfloat) Na * 0.5f - 8.0f;
    double delt = (double) Nb / (double) Na * 0.005;
    std::vector<GLfloat> R;
    
    rotate_z(theta_a, R);
    R[3] = delx; // shift on x axis
    R[7] = 4.0f; // shift on y axis
    //GL_TRUE is row major order
    glUniformMatrix4fv(uniRotate, 1, GL_TRUE, R.data());
    theta_a += delt;
}

void SetUp::rotate_b()
{
    static const GLfloat delx = -8.0f;
    std::vector<GLfloat> R;
    
    rotate_z(theta_b, R);
    R[3] = delx;
    R[7] = 4.0f; // shift on y axis
    //GL_TRUE is row major order
    glUniformMatrix4fv(uniRotate, 1, GL_TRUE, R.data());
    theta_b -= 0.005;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(1200, 900, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(1920, 1200, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);
    // initialise GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    //GlewSetupCheck();
    auto mySetUp = std::make_unique<SetUp>();
    GLuint n1inds_a = mySetUp -> GetN1Inds_a();
    GLuint ninds_a = mySetUp -> GetNInds_a();
    GLuint n1inds_b = mySetUp -> GetN1Inds_b();
    GLuint ninds_b = mySetUp -> GetNInds_b();
    
    std::cout << "\nOpenGL core profile version string: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL vendor string: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL renderer string: " << glGetString(GL_RENDERER) << std::endl;
    
	auto t_start = std::chrono::high_resolution_clock::now();
    while(!glfwWindowShouldClose(window)){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // draw first gear
        mySetUp -> rotate_a();
        // set color
        mySetUp -> setColor(0.1f, 0.2f, 0.5f);
        glDrawElements(GL_TRIANGLES, n1inds_a, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
        //GLuint ni1 = 24 * 90;
        //glDrawElements(GL_TRIANGLES, ni1, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
        mySetUp -> setColor(0.184314, 0.309804, 0.184314); // dark green
        //mySetUp -> setColor(0.33f, 0.42f, 0.18f); // olive green
        //mySetUp -> setColor(0.0f, 1.0f, 0.0f); // green
        //mySetUp -> setColor(0.184314, 0.309804, 0.184314); // dark green
        glDrawElements(GL_TRIANGLES, ninds_a - n1inds_a, GL_UNSIGNED_INT, (void*)(n1inds_a * sizeof(GLuint)));
        
        // draw second gear
        mySetUp -> rotate_b();
        // set color
        mySetUp -> setColor(0.1f, 0.1f, 0.4f);
        glDrawElements(GL_TRIANGLES, n1inds_b, GL_UNSIGNED_INT, (void*)(ninds_a * sizeof(GLuint)));
        mySetUp -> setColor(0.25f, 0.25f, 0.25f); // grey 
        glDrawElements(GL_TRIANGLES, ninds_b - n1inds_b, GL_UNSIGNED_INT, (void*)((ninds_a + n1inds_b) * sizeof(GLuint)));
        
		// sleep until 50 milli seconds is reached
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        // 20 frames per second, acceptable
        std::this_thread::sleep_for(std::chrono::milliseconds(50 - (int) (time * 1000)));
		t_start = std::chrono::high_resolution_clock::now();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}


