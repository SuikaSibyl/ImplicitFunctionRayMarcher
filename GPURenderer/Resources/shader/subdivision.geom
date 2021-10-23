#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 200) out;

in VS_OUT {
    vec2 TexCoords;

    vec3 MV_Normal;
    vec4 MV_FragPos;
    vec3 MV_LightPos;

    mat4 Project;
} gs_in[];

struct Vertex
{
    vec4 Position;
    vec2 TexCoords;

    vec3 MV_Normal;
    vec4 MV_FragPos;
    vec3 MV_LightPos;

    mat4 Project;
};

struct Triangle
{
    Vertex vertex[3];
};

out vec2 TexCoords;

out vec3 MV_Normal;
out vec3 MV_FragPos;
out vec3 MV_LightPos;

uniform int subnum;

void EmitTriangle(Triangle triangle)
{
    for(int i=0; i<3; i++)
    {
        gl_Position = gs_in[0].Project * triangle.vertex[i].Position;  

        TexCoords = triangle.vertex[i].TexCoords;
        MV_Normal = triangle.vertex[i].MV_Normal;
        MV_FragPos = vec3(triangle.vertex[i].MV_FragPos);
        MV_LightPos = triangle.vertex[i].MV_LightPos;

        EmitVertex();
    }
    EndPrimitive();
}

void RecursiveSubdivision(Triangle triangle, int depth, int maxDepth)
{
    int queue_back = 0;
    int queue_front = 0;
    Triangle triangle_queue[100];
    triangle_queue[queue_back++] = triangle;

    while(true)
    {
        if(depth==maxDepth)
        {
            for(int i = queue_front; i<queue_back;i++)
            {
                EmitTriangle(triangle_queue[i]);
            }
            break;
        }

        int pre_queue_front = queue_front;
        int pre_queue_back = queue_back;
        for(int i=pre_queue_front; i<pre_queue_back; i++)
        {
            Vertex interpolation[3];
            for(int j=0; j<3; j++)
            {
                int left = j;
                int right = j+1;
                // Special case
                if(right==3)
                    right=0;
                // Find the Mid point
                interpolation[j].TexCoords = (triangle_queue[i].vertex[left].TexCoords + triangle_queue[i].vertex[right].TexCoords)/2;
                interpolation[j].MV_Normal = normalize((triangle_queue[i].vertex[left].MV_Normal + triangle_queue[i].vertex[right].MV_Normal)/2);
                interpolation[j].MV_FragPos = (triangle_queue[i].vertex[left].MV_FragPos + triangle_queue[i].vertex[right].MV_FragPos)/2;
                interpolation[j].MV_LightPos = (triangle_queue[i].vertex[left].MV_LightPos + triangle_queue[i].vertex[right].MV_LightPos)/2;

                float cos_2theta = dot(triangle_queue[i].vertex[left].MV_Normal, triangle_queue[i].vertex[right].MV_Normal);
                float cos_theta = sqrt((1+cos_2theta)/2);
                float sin_theta = sqrt((1-cos_2theta)/2);
                float tan_theta = sin_theta/cos_theta;
                vec3 half_vec = (vec3(triangle_queue[i].vertex[right].Position) - vec3(triangle_queue[i].vertex[left].Position))/2;
                float half_dis = length(half_vec);
                vec4 half_pos = (triangle_queue[i].vertex[left].Position + triangle_queue[i].vertex[right].Position)/2;
                float ext_dis = half_dis/sin_theta - half_dis/tan_theta;
                interpolation[j].Position = half_pos + vec4(interpolation[j].MV_Normal * ext_dis, 0.0);
            }
            queue_front++;
            Triangle temp;
            temp.vertex[0] = interpolation[0];
            temp.vertex[1] = interpolation[1];
            temp.vertex[2] = interpolation[2];
            triangle_queue[queue_back++] = temp;
            for(int j=0; j<3; j++)
            {
                int left = j;
                int right = j+1;
                // Special case
                if(right==3)
                    right=0;
                // Find the Mid point
                temp.vertex[0] = triangle_queue[i].vertex[right];
                temp.vertex[1] = interpolation[left];
                temp.vertex[2] = interpolation[right];
                triangle_queue[queue_back++] = temp;
            }
        }
        depth++;
        // triangle = interpolation;
    }
}
// ==========================
void main()  
{
    // Initialize the triangle
    Triangle triangle;
    for(int i = 0; i<3; i++)
    {
        triangle.vertex[i].Position = gs_in[i].MV_FragPos;
        triangle.vertex[i].TexCoords = gs_in[i].TexCoords;
        triangle.vertex[i].MV_Normal = gs_in[i].MV_Normal;
        triangle.vertex[i].MV_FragPos = gs_in[i].MV_FragPos;
        triangle.vertex[i].MV_LightPos = gs_in[i].MV_LightPos;
    }
    
    // Recursive Draw
    RecursiveSubdivision(triangle, 0, subnum);
    EndPrimitive();  
}  