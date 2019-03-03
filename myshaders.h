#ifndef MYSHADERS_H
#define MYSHADERS_H

#endif // MYSHADERS_H

static const char *vertexShaderSourceNew = R"glsl(
    #version 330
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    out vec3 Normal, FragPos;
    uniform mat4 matrix;
    uniform mat4 perspective;
    uniform mat4 rot;

    void main()
    {
       gl_Position = perspective * matrix * vec4(aPos, 1.0);
       Normal = vec3(rot * vec4(aNormal, 0.0));
       FragPos = vec3(matrix * vec4(aPos, 1.0));
    }
)glsl";

static const char *fragmentShaderSourceNew = R"glsl(
    #version 330
    in vec3 Normal;
    in vec3 FragPos;
    out vec4 outColor;
    uniform vec3 triangleColor;
    uniform vec3 lightPos;
    void main()
    {
        vec3 lightColor = vec3(0.9, 0.9, 0.9);
        float ambientStrength = 0.2;
        vec3 ambient = ambientStrength * lightColor;
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * triangleColor;
        outColor = vec4(result, 1.0);
    }
)glsl";

static const char *vertexShaderSource = R"glsl(
    #version 130
    in vec3 aPos;
    in vec3 aNormal;
    out vec3 Normal, FragPos;
    uniform mat4 matrix;
    uniform mat4 perspective;
    uniform mat4 rot;

    void main()
    {
       gl_Position = perspective * matrix * vec4(aPos, 1.0);
       Normal = vec3(rot * vec4(aNormal, 0.0));
       FragPos = vec3(matrix * vec4(aPos, 1.0));
    }
)glsl";

static const char *fragmentShaderSource = R"glsl(
    #version 130
    in vec3 Normal;
    in vec3 FragPos;
    out vec4 outColor;
    uniform vec3 triangleColor;
    uniform vec3 lightPos;
    void main()
    {
        vec3 lightColor = vec3(0.9, 0.9, 0.9);
        float ambientStrength = 0.2;
        vec3 ambient = ambientStrength * lightColor;
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * triangleColor;
        outColor = vec4(result, 1.0);
    }
)glsl";
