#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 24) out;

in VS_OUT {
    vec2 TexCoords;

    vec3 MV_Normal;
    vec3 MV_FragPos;
    vec3 MV_LightPos;

    vec4 expand;
    int Pass;

    bool backFace;
} gs_in[];

uniform int pass;

// ==========================
out vec2 TexCoords;

out vec3 MV_Normal;
out vec3 MV_FragPos;
out vec3 MV_LightPos;

out int Pass;  

void EmitRaw(int i)
{
    gl_Position = gl_in[i].gl_Position;  

    MV_Normal = gs_in[i].MV_Normal;
    MV_FragPos = gs_in[i].MV_FragPos;
    MV_LightPos = gs_in[i].MV_LightPos;
    Pass = gs_in[i].Pass;
        
    EmitVertex();
}

void EmitExpand(int i)
{
    gl_Position = gs_in[i].expand;  

    MV_Normal = gs_in[i].MV_Normal;
    MV_FragPos = gs_in[i].MV_FragPos;
    MV_LightPos = gs_in[i].MV_LightPos;
    Pass = gs_in[i].Pass;
    
    EmitVertex();
}
// ==========================
void main()  
{  
    if(gs_in[0].Pass == 1)
    {
        if(gs_in[0].backFace || gs_in[1].backFace||gs_in[2].backFace)
        {
            EmitExpand(0);
            EmitExpand(1);
            EmitExpand(2);
            EndPrimitive();  
            
            EmitExpand(1);
            EmitExpand(2);
            EmitRaw(2);
            EndPrimitive(); 

            EmitExpand(2);
            EmitRaw(2);
            EmitExpand(0);
            EndPrimitive();  

            EmitRaw(2);
            EmitRaw(0);
            EmitExpand(0);
            EndPrimitive();  

            EmitExpand(0);
            EmitRaw(0);
            EmitExpand(1);
            EndPrimitive();  

            EmitRaw(0);
            EmitExpand(1);
            EmitRaw(1);
            EndPrimitive();  

            EmitExpand(1);
            EmitRaw(1);
            EmitRaw(2);
            EndPrimitive();  

            EmitRaw(1);
            EmitRaw(2);
            EmitRaw(0);
            EndPrimitive();  
        }
    }
    else if(gs_in[0].Pass == 2)
    {
        EmitRaw(0);
        EmitRaw(1);
        EmitRaw(2);
        EndPrimitive();  
    }
}  