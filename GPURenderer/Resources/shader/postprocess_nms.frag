#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
const float offset_x = 1.0 / 1280.0;  
const float offset_y = 1.0 / 720.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset_x,  offset_y), // 左上
        vec2( 0.0f,    offset_y), // 正上
        vec2( offset_x,  offset_y), // 右上
        vec2(-offset_x,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset_x,  0.0f),   // 右
        vec2(-offset_x, -offset_y), // 左下
        vec2( 0.0f,   -offset_y), // 正下
        vec2( offset_x, -offset_y)  // 右下
    );

    float sampleTex[9];
    for(int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i])).r;

    bool isLocalMaximum = true;
    for(int i = 0; i < 9; i++)
    {
        if(sampleTex[4]<sampleTex[i])
        {
            isLocalMaximum = false;
            break;
        }
    }

    float threshold = 0.5;
    if(TexCoords.x<0.001 || TexCoords.x>0.999 || TexCoords.y<0.001 || TexCoords.y>0.999)
        FragColor = vec4(vec3(0), 1.0);
    else if(isLocalMaximum == true && sampleTex[4]>threshold)
        FragColor = vec4(vec3(1), 1.0);
    else
        FragColor = vec4(vec3(0), 1.0);
}