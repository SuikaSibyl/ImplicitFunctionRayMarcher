#version 330 core

// Input & Output
in vec2 TexCoords;
out vec4 FragColor;

// Uniform Datas
uniform vec2 iResolution;
uniform vec3 camPos;
uniform float currentFrame;

// Uniform Texture Datas
uniform samplerCube skybox;

// Uniform Matrix Datas
uniform mat4 invView;
uniform mat4 invProjection;
uniform mat4 ModelViewProjection;

uniform bool useGlass;

float ground_y = -0.12;

//----------------------------------------------------------------
// SDF Functions
//----------------------------------------------------------------

float Torus(vec3 pos)
{   
    float tmp = 1 - length(pos.xy);
    return tmp * tmp + pos.z * pos.z - 0.1;
}

float Steiner(vec3 pos)
{   
    float para1 = 1 + sin(currentFrame);
    float para2 = 1 + sin(2*currentFrame);
    float para3 = 1 + sin(4*currentFrame);
    pos = pos * 10;
    float x2 = pos.x * pos.x;
    float y2 = pos.y * pos.y;
    float z2 = pos.z * pos.z;
    return x2*y2*para1 + x2 * z2*para2 + y2 *z2 * para3 - 3 * pos.x * pos.y * pos.z;
}

float Heart(vec3 pos)
{
    pos.xyz = pos.xzy;
    pos.y = pos.y/1.5;
    pos = pos * 3;

    float x2 = pos.x * pos.x;
    float y2 = pos.y * pos.y;
    float z2 = pos.z * pos.z;
    
    float poly = (2*x2 + 2*y2 + z2 - 1);
    poly = poly * poly * poly;
    return poly - 0.1 * x2*z2*pos.z - y2 *z2*pos.z;
}

float Tooth(vec3 pos)
{
    float para1 = 1 + sin(3 * currentFrame);
    pos = pos * 3;

    float x2 = pos.x * pos.x;
    float y2 = pos.y * pos.y;
    float z2 = pos.z * pos.z;
    
    return x2 * x2 + y2 * y2 + z2 * z2 - x2 - y2 - z2;
}

float Superquadric(vec3 pos, float num)
{
    pos = pos * 5;
    return pow(abs(pos.x),num) + pow(abs(pos.y),num) + pow(abs(pos.z),num) - 1;
}

float Ground(vec3 pos)
{
    return pos.y;
}

float ScherkMinimal(in vec3 pos)
{
    pos = pos * 50;
    return exp(pos.z) * cos(pos.y) - cos(pos.x);
}

float Barth(in vec3 pos)
{
    pos = pos * 100;
    float phi = 1.618;
    float phi2 = phi * phi;
    float x2 = pos.x * pos.x;
    float y2 = pos.x * pos.y;
    float z2 = pos.x * pos.z;
    float tmp = x2 + y2 + z2 - 1;
    return 4 * (phi * x2 - y2) * (phi2 * y2 - z2)*(phi2 * z2 - x2) - (1+2*phi)*tmp*tmp;
}

float GetPotential(in vec3 p)
{
    float alpha = 0.5 + 0.5 * sin(0.3*currentFrame);
    return (1 - alpha) * Heart(p) + alpha * (Tooth(p));
}

const float precis=.0001;

struct Out_RayAABB
{
    bool isCollide;
    vec2 dis;
};

Out_RayAABB RayAABB(in vec3 ro,in vec3 rd)
{
    vec3 amax = vec3(1) * 0.5;
    vec3 amin = vec3(-1) * 0.5;

    float tmin = 0.0;
    float tmax = 1e6;

    // Get intersect points on three axis
    for (int i = 0; i < 3; i++)
    {
        if (abs(rd[i]) == 0)
        {
            // Parallel with the axis
            continue;
        }
        else{
            // Cal AABB
            float ood = 1.0f / rd[i];
            float t1 = (amin[i] - ro[i]) * ood;
            float t2 = (amax[i] - ro[i]) * ood;
            if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax) return Out_RayAABB(false,vec2(-1,-1));
        }
    }
    return Out_RayAABB(true,vec2(tmin,tmax));
}

vec3 calcNormal(in vec3 pos)
{
    vec3 eps=vec3(precis,0.,0.);
    return normalize(vec3(
            GetPotential(pos+eps.xyy)-GetPotential(pos-eps.xyy),
            GetPotential(pos+eps.yxy)-GetPotential(pos-eps.yxy),
            GetPotential(pos+eps.yyx)-GetPotential(pos-eps.yyx)));
            
}

float SihouetteAdaption(vec3 pos)
{
    vec3 d = normalize(camPos - pos);
    return abs(dot(d,calcNormal(pos)));
}

float GetStepLength(vec3 pos, float potential)
{
    potential = abs(potential);

    if(potential <= 0.1)
    {
        return 0.00001f;
    }
    else if(potential <= 1)
    {
        return 0.001f;
    }
    else if(potential <= 10)
    {
        return 0.01f;
    }
    else if(potential <=20)
    {
        return 0.02f;
    }
    else if(potential <=50)
        return 0.05f;
    else if(potential <=100)
        return 0.1;
    else
        return 0.5;
}


struct Out_Raymarch
{
    bool isCollide;
    float dis;
};

Out_Raymarch Raymarch(in vec3 ro,in vec3 rd)
{
    Out_RayAABB outRayAABB = RayAABB(ro, rd);

    if(outRayAABB.isCollide)
    {
        float dmax = outRayAABB.dis[1];
        float d = outRayAABB.dis[0];
        float potential = GetPotential(ro + d * rd);
        float potential_old = potential;
        float s = 0.5;

        while(d < dmax)
        {   
            s = GetStepLength(ro + d * rd, potential_old);
            potential = GetPotential (ro + ( d + s ) * rd);

            // If find root
            if(potential*potential_old < 0)
            {
                float left = d;
                float right = d+s;
                float mid = (left + right)/2;

                float potential_left = potential_old;
                float potential_right = potential;

                for(int i = 0; i < 10; i++)
                {
                    potential = GetPotential (ro + mid * rd);

                    if(potential_left*potential < 0)
                    {
                        right = mid;
                        mid = (left + right)/2;
                        potential_right = potential;
                    }
                    else
                    {
                        left = mid;
                        mid = (left + right)/2;
                        potential_left = potential;
                    }
                }
                return Out_Raymarch(true, mid);
            }
            potential_old = potential;
            d += s;
        }

        return Out_Raymarch(false, -1);
    }
    else
    {
        return Out_Raymarch(false, -1);
    }
}


struct Out_ShadowMarch
{
    bool isCollide;
    float dis;
    float res;
};

Out_ShadowMarch ShadowMarch(in vec3 ro,in vec3 rd)
{
    Out_RayAABB outRayAABB = RayAABB(ro, rd);

    if(outRayAABB.isCollide)
    {
        float dmax = outRayAABB.dis[1];
        float d = outRayAABB.dis[0];
        float potential = GetPotential(ro + d * rd);
        float potential_old = potential;
        float s = 0.5;

        float res = 1.0;
        
        while(d < dmax)
        {   
            s = GetStepLength(ro + d * rd, potential_old);
            potential = GetPotential (ro + ( d + s ) * rd);

            // If find root
            if(potential*potential_old < 0)
            {
                float left = d;
                float right = d+s;
                float mid = (left + right)/2;

                float potential_left = potential_old;
                float potential_right = potential;

                for(int i = 0; i < 10; i++)
                {
                    potential = GetPotential (ro + mid * rd);

                    if(potential_left*potential < 0)
                    {
                        right = mid;
                        mid = (left + right)/2;
                        potential_right = potential;
                    }
                    else
                    {
                        left = mid;
                        mid = (left + right)/2;
                        potential_left = potential;
                    }
                }
                return Out_ShadowMarch(true, mid, 0.0);
            }
            d += s;
            res = min(res, abs(potential)*0.4 / d);
            potential_old = potential;
        }

        return Out_ShadowMarch(false, -1, res);
    }
    else
    {
        return Out_ShadowMarch(false, -1, -1);
    }
}

float calcDepth(vec3 localPos,float cos){
    vec4 frag=ModelViewProjection*vec4(localPos,1);
    frag/=frag.w;
    return(frag.z+1)/2;
}

void main()
{
    //-----------------------------------------------------
    // montecarlo (over time, image plane and lens) (5D)
    //-----------------------------------------------------
    vec3 tot=vec3(0.);
    
    //-----------------------------------------------------
    // animate scene
    //-----------------------------------------------------
    float time=currentFrame;
    
    //-----------------------------------------------------
    // Initialize Camera
    //-----------------------------------------------------
    
    // image plane
    vec2 uv=2.*gl_FragCoord.xy/iResolution.xy-1.;
    vec3 ro=vec3(invView*vec4(0,0,0,1));
    
    // Get Ray Direction
    vec4 camdir=invProjection*vec4(uv,1,1);
    camdir=camdir/camdir.w;
    vec3 dir=mat3(invView)*vec3(camdir);
    vec3 rd=normalize(dir);
    
    //-----------------------------------------------------
    // render
    //-----------------------------------------------------

    // raymarch
    Out_Raymarch out_raymarch=Raymarch(ro,rd);
    if(out_raymarch.isCollide)
    {
        vec3 col=vec3(0);

        // geometry
        vec3 pos=ro+out_raymarch.dis*rd;
        vec3 nor=calcNormal(pos);
        vec3 ref=reflect(rd,nor);
        
        // materials
        vec3 mate=vec3(1.);
        
        // lighting
        vec3 lin = vec3(0.0);
        lin += mix( vec3(0.05,0.02,0.0), 1.2*vec3(0.8,0.9,1.0), 0.5 + 0.5*nor.y );
        lin *= 1.0 + 1.5*vec3(0.7,0.5,0.3)*pow( clamp( 1.0 + dot(nor,rd), 0.0, 1.0 ), 2.0 );
        lin += 1.5*clamp(0.3+2.0*nor.y,0.0,1.0)*pow((texture(skybox, ref)).xyz,vec3(2.2))*(0.04+0.96*pow( clamp( 1.0 + dot(nor,rd), 0.0, 1.0 ), 4.0 ));

        float ambient = 1;
        vec3 shadow_ro = ro+out_raymarch.dis*rd;
        vec3 shadow_rd = normalize(vec3(1) - shadow_ro);
        shadow_ro = shadow_ro+ 0.0001*shadow_rd;

        vec3 L = vec3(1) - shadow_ro;
        float LdN = max(0, dot(nor, L));
        
        ambient = LdN;

        ambient = 1-(0.8*(1-ambient));


        col=mate * lin * ambient;
        
        gl_FragDepth=calcDepth(pos,rd.z);

        if(useGlass)
        {
            vec3 lightDir = normalize(vec3(1) - pos);
            vec3 viewDir = -rd;
            vec3 halfDir = (lightDir + viewDir) /2;
            float spec = pow(max(dot(nor, halfDir), 0.0), 32);
            col =texture(skybox, normalize(ref)).xyz;
            vec3 refract = refract(normalize(-rd), normalize(nor), 1.33);
            if(refract!=vec3(0))
            {
                vec3 AA = texture(skybox, normalize(refract)).xyz;
                col = 0.5 * ((1-LdN)*col + LdN * AA) + 0.5 * AA;
            }
            float fernel = 1 - max(0, dot(-nor, rd));
            fernel = fernel * fernel;
            // fernel = 0;
            col = fernel * col*2 + (1-fernel)*texture(skybox, normalize(rd)).xyz + spec;
        }
        else
        {
            vec3 lightDir = normalize(vec3(1) - pos);
            vec3 viewDir = -rd;
            vec3 halfDir = (lightDir + viewDir) /2;
            float spec = pow(max(dot(nor, halfDir), 0.0), 32);
            col += spec;
            
            // Gamma Correction
            col=pow(clamp(col,0.,1.),vec3(.45));
        }

        //-----------------------------------------------------
        // postprocessing
        //-----------------------------------------------------
        FragColor=vec4(col,1.);
    }
    else
    {
        
        discard;
    }
}