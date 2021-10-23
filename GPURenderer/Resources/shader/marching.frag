#version 330 core

#define numballs 4
#define MAX_ITER 200

const float EPSILON = .0001;

const float STEP_SIZE = .05;

// Input & Output
in vec2 TexCoords;
out vec4 FragColor;

// Uniform Datas
uniform vec2 iResolution;
uniform vec3 camPos;
uniform float currentFrame;

// Uniform Texture Datas
uniform samplerCube skybox;

// Uniform Array Datas
uniform vec4 blobs[numballs];

// Uniform Matrix Datas
uniform mat4 invView;
uniform mat4 invProjection;
uniform mat4 ModelViewProjection;

//----------------------------------------------------------------
// Hash Functions
//----------------------------------------------------------------

float hash1(float n)
{
    return fract(sin(n)*43758.5453123);
}

vec2 hash2(float n)
{
    return fract(sin(vec2(n,n+1.))*vec2(43758.5453123,22578.1459123));
}

vec3 hash3(float n)
{
    return fract(sin(vec3(n,n+1.,n+2.))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

//----------------------------------------------------------------
// SDF Functions
//----------------------------------------------------------------

float sdMetaBalls(vec3 pos)
{   
    float sumDensity=0;
    float sumRi=0;
    float minDistance=1e20;
    for(int i=0;i<numballs;i++)
    {
        float r=length(blobs[i].xyz-pos);
        if(r<=blobs[i].w){
            // this can be factored for speed if you want
            sumDensity+=2*(r*r*r)/(blobs[i].w*blobs[i].w*blobs[i].w)-
            3*(r*r)/(blobs[i].w*blobs[i].w)+1;
        }
        minDistance=min(minDistance,r-blobs[i].w);
        sumRi+=blobs[i].w;
    }
    
    float magic=.25;
    return(magic-sumDensity)/(3/2.f*sumRi);
}

float map(in vec3 p)
{
    return sdMetaBalls(p);
}

const float precis=.0001;

vec2 Raymarch(in vec3 ro,in vec3 rd)
{
    float maxd=50.;
    float d = .0;
    float t = .0;
    float m=1.;
    int iter = 0;
    for (; iter < MAX_ITER; ++iter) {
        t = map (ro + d * rd);
        if (t < EPSILON*(1. + .125*t)) break;
        d += t*STEP_SIZE;
    }

    if(d>maxd || iter == MAX_ITER)m=-1.;
    return vec2(d,m);
}

vec3 norMetaBalls( vec3 pos )
{
	vec3 nor = vec3( 0.0, 0.0001, 0.0 );
		
	for( int i=0; i<numballs; i++ )
	{
        float db = length( blobs[i].xyz - pos );
		float x = clamp( db/blobs[i].w, 0.0, 1.0 );
		float p = 6 * x - 6 * x * x;
		nor += normalize( pos - blobs[i].xyz ) * p / blobs[i].w;
	}
	
	return normalize( nor );
}

vec3 calcNormal(in vec3 pos)
{
    vec3 eps=vec3(precis,0.,0.);
    // return normalize(vec3(
    //         map(pos+eps.xyy)-map(pos-eps.xyy),
    //         map(pos+eps.yxy)-map(pos-eps.yxy),
    //         map(pos+eps.yyx)-map(pos-eps.yyx)));
    return norMetaBalls(pos);
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
    
    vec3 col=vec3(0);
    
    // vec3 ro = camPos;
    // raymarch
    vec2 tmat=Raymarch(ro,rd);
    // If Do intersect
    if(tmat.y>-.5)
    {
        // geometry
        vec3 pos=ro+tmat.x*rd;
        vec3 nor=calcNormal(pos);
        vec3 ref=reflect(rd,nor);
        
        // materials
        vec3 mate=vec3(1.);
        
        // lightingl
        vec3 lin=vec3(0.);
        
        lin+=mix(vec3(.05,.02,0.),1.2*vec3(.8,.9,1.),.5+.5*nor.y);
        lin*=1.+1.5*vec3(.7,.5,.3)*pow(clamp(1.+dot(nor,rd),0.,1.),2.);
        lin+=1.5*clamp(.3+2.*nor.y,0.,1.)*pow((texture(skybox,ref)).xyz,vec3(2.2))*(.04+.96*pow(clamp(1.+dot(nor,rd),0.,1.),4.));
        
        col=mate*lin;
        
        gl_FragDepth=calcDepth(pos,rd.z);
    }
    // If Not intersect
    else
    {
        discard;
    }
    
    // Gamma Correction
    col=pow(clamp(col,0.,1.),vec3(.45));
    tot+=col;
    
    //-----------------------------------------------------
    // postprocessing
    //-----------------------------------------------------
    FragColor=vec4(tot,1.);
    
}