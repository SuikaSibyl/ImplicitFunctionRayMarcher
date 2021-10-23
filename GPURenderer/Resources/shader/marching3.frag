#version 330 core

#define AA 1

#define numballs 6

// undefine this for numerical normals
#define ANALYTIC_NORMALS

out vec4 FragColor;

in vec2 TexCoords;

uniform samplerCube skybox;

uniform vec2 iResolution;
uniform vec3 camPos;
uniform float currentFrame;
uniform mat4 invView;
uniform mat4 invProjection;
uniform mat4 ModelViewProjection;

uniform vec4 blobs[numballs];

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 50.0f; // 投影矩阵的远平面

//----------------------------------------------------------------
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}


//----------------------------------------------------------------

float hash1( float n )
{
    return fract(sin(n)*43758.5453123);
}

vec2 hash2( float n )
{
    return fract(sin(vec2(n,n+1.0))*vec2(43758.5453123,22578.1459123));
}

vec3 hash3( float n )
{
    return fract(sin(vec3(n,n+1.0,n+2.0))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

//----------------------------------------------------------------


float sdMetaBalls( vec3 pos )
{
	float m = 0.0;
	float p = 0.0;
	float dmin = 1e20;
		
	float h = 1.0; // track Lipschitz constant
	
	for( int i=0; i<numballs; i++ )
	{
		// bounding sphere for ball
        float db = length( blobs[i].xyz - pos );
        if( db < blobs[i].w )
    	{
    		float x = db/blobs[i].w;
    		p += 1.0 - x*x*x*(x*(x*6.0-15.0)+10.0);
	    	m += 1.0;
    		h = max( h, 0.5333*blobs[i].w );
    		dmin = min( dmin, db - blobs[i].w );
	    }
	    else // bouncing sphere distance
	    {
    		dmin = min( dmin, db - blobs[i].w );
    	}
	}
    float d = dmin + 0.04;
	
	// if( m>0.5 )
	// {
	// 	float th = 0.0005;
	// 	d = h*(th-p);
	// }
	
	return d;
}


vec3 norMetaBalls( vec3 pos )
{
	vec3 nor = vec3( 0.0, 0.0001, 0.0 );
		
	for( int i=0; i<numballs; i++ )
	{
        float db = length( blobs[i].xyz - pos );
		float x = clamp( db/blobs[i].w, 0.0, 1.0 );
		float p = x*x*(30.0*x*x - 60.0*x + 30.0);
		nor += normalize( pos - blobs[i].xyz ) * p / blobs[i].w;
	}
	
	return normalize( nor );
}


float map( in vec3 p )
{
	return sdMetaBalls( p );
}


const float precis = 0.00001;

vec2 intersect( in vec3 ro, in vec3 rd )
{
	float maxd = 10.0;
    float h = precis*2.0;
    float t = 0.0;
    float m = 1.0;
    for( int i=0; i<200; i++ )
    {
        if( h<precis||t>maxd ) continue;//break;
        t += h;
	    h = map( ro+rd*t );
    }

    if( t>maxd ) m=-1.0;
    return vec2( t, m );
}

vec3 calcNormal( in vec3 pos )
{
#ifdef ANALYTIC_NORMALS	
	return norMetaBalls( pos );
#else	
    vec3 eps = vec3(precis,0.0,0.0);
	return normalize( vec3(
           map(pos+eps.xyy) - map(pos-eps.xyy),
           map(pos+eps.yxy) - map(pos-eps.yxy),
           map(pos+eps.yyx) - map(pos-eps.yyx) ) );
#endif
}

float procDepth(vec3 localPos){
    vec4 frag = ModelViewProjection*vec4(localPos,1);
    frag/=frag.w;
    return (frag.z+1)/2;
}

void main()
{
    //-----------------------------------------------------
    // input
    //-----------------------------------------------------
	vec2 m = vec2(0.5);

    //-----------------------------------------------------
    // montecarlo (over time, image plane and lens) (5D)
    //-----------------------------------------------------

	vec3 tot = vec3(0.0);

    float toff = 0.0;

    //-----------------------------------------------------
    // animate scene
    //-----------------------------------------------------
    float time = currentFrame + toff;

    // move metaballs
    // for( int i=0; i<numballs; i++ )
    // {
    //     float h = float(i)/8.0;
    //     blobs[i].xyz = 2.0*sin( 6.2831*hash3(h*1.17) + hash3(h*13.7)*time );
    //     blobs[i].w = 1.7 + 0.9*sin(6.28*hash1(h*23.13));
    // }
    // blobs[0].w = 0.2;
    // paras[0].xy = vec2(1);
    // paras[0].xy = vec2(1);
    // blobs[1].xyzw = vec4(-0.0002226,0.71570887, 0.0401321, 0.109749);
    
    // blobs[0] = vec4(2.59957426e-04, 3.72819252e-01, -4.08866702e-02, 9.19603546e-02);
    // blobs[1] = vec4(2.226e-04,0.71570887, -1.58953472e-02, 1.93718548e-01);
    // // blobs[1].xyzw = vec4(1.68051201e-04, 6.47908326e-01, -1.23883727e-01, 1.93718548e-01);
    // blobs[2] = vec4(1.30178017e-04, 6.55445550e-01, -1.58953472e-02, 4.31896888e-02);
    // blobs[3] = vec4(1.96820652e-04, -7.13649021e-02, -8.80288768e-02, 8.31114112e-02);
    // blobs[4] = vec4(1.74016762e-04, 3.76585243e-01, -8.13657325e-02, 5.47158545e-02);
    // blobs[5] = vec4(1.38738082e-04, 8.60169618e-01, 2.93185700e-02, 8.07801561e-02);
    // blobs[6] = vec4(2.06530016e-04, 7.91870401e-01, 1.39823627e-02, 2.37796503e-02);
        // blobs[0].xyzw = vec4(0.000139012, 0.737, -0.1079, 0.248224185);
    // blobs[1].xyzw = vec4(0.0004541, 0.478523, 0.04995, 0.1434);
    // paras[1].xy = vec2(1);
    //-----------------------------------------------------
    // Initialize Camera
    //-----------------------------------------------------

    // image plane
    vec2 uv = 2.0*gl_FragCoord.xy/iResolution.xy-1.0;
    vec3 ro = vec3(invView*vec4(0,0,0,1));

    vec4 camdir = invProjection * vec4(uv,1,1);
    camdir = camdir/camdir.w;//W归一后 得出相机坐标体系下的点
    vec3 dir = mat3(invView)*vec3(camdir);
    vec3 rd = normalize(dir);

    //-----------------------------------------------------
    // render
    //-----------------------------------------------------

    // background
    // vec3 col = pow( texture( iChannel0, rd ).xyz, vec3(2.2) );
    vec3 col = vec3(0);
    
    // vec3 ro = camPos;
    // raymarch
    vec2 tmat = intersect(ro,rd);
    if( tmat.y>-0.5 )
    {
        // geometry
        vec3 pos = ro + tmat.x*rd;
        vec3 nor = calcNormal(pos);
        vec3 ref = reflect( rd, nor );

        // materials
        vec3 mate = vec3(1.0);
        // float w = 0.01;
        // for( int i=0; i<numballs; i++ )
        // {
        //     float h = float(i)/8.0;

        //     // metaball color
        //     vec3 ccc = vec3(1.0);
        //     ccc = mix( ccc, vec3(1.0,0.60,0.05), smoothstep(0.65,0.66,sin(30.0*h)));
        //     ccc = mix( ccc, vec3(0.3,0.45,0.25), smoothstep(0.65,0.66,sin(15.0*h)));
        
        //     float x = clamp( length( blobs[i].xyz - pos )/blobs[i].w, 0.0, 1.0 );
        //     float p = 1.0 - x*x*(3.0-2.0*x);
        //     mate += p*ccc;
        //     w += p;
        // }
        // mate /= w;

        // lightingl
        vec3 lin = vec3(0.0);
        
        lin += mix( vec3(0.05,0.02,0.0), 1.2*vec3(0.8,0.9,1.0), 0.5 + 0.5*nor.y );
        lin *= 1.0 + 1.5*vec3(0.7,0.5,0.3)*pow( clamp( 1.0 + dot(nor,rd), 0.0, 1.0 ), 2.0 );
        lin += 1.5*clamp(0.3+2.0*nor.y,0.0,1.0)*pow((texture(skybox, ref)).xyz,vec3(2.2))*(0.04+0.96*pow( clamp( 1.0 + dot(nor,rd), 0.0, 1.0 ), 4.0 ));

        // mate *= vec3(texture(skybox, rd));
        // surface-light interacion
        // col = mate * lin;
        col = mate;
        // col = mate;
    }
    // gamma
    col = pow( clamp(col,0.0,1.0), vec3(0.45) );
    tot += col;
    
	//-----------------------------------------------------
	// postprocessing
    //-----------------------------------------------------

	// vignetting
	vec2 q = gl_FragCoord.xy / iResolution.xy;
    tot *= 0.5 + 0.5*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );
    if(tot.rgb == vec3(0))
        // float x = 0;
        discard;
    FragColor = vec4(tot, 1.0);
    
}