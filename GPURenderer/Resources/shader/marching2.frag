#version 330 core

#define AA 1

#define numballs 2

// undefine this for numerical normals
// #define ANALYTIC_NORMALS

out vec4 FragColor;

in vec2 TexCoords;

uniform samplerCube skybox;

uniform vec2 iResolution;
uniform vec3 camPos;
uniform float currentFrame;
uniform mat4 invView;
uniform mat4 invProjection;

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

vec4 blobs[numballs];

float sdMetaBalls( vec3 pos )
{
	float m = 0.0;
	float p = 0.0;
	float dmin = 1e20;
		
	float h = 1.0; // track Lipschitz constant
	
	for( int i=0; i<numballs; i++ )
	{
        if(blobs[i].w < 0)
            continue;
		// bounding sphere for ball
        float db = length( blobs[i].xyz - pos );
        dmin = min( dmin, db - blobs[i].w );
        // if( db < blobs[i].w )
    	// {
    	// 	float x = db/blobs[i].w;
    	// 	p += 1.0 - x*x*x*(x*(x*6.0-15.0)+10.0);
	    // 	m += 1.0;
    	// 	h = max( h, 0.5333*blobs[i].w );
    	// 	dmin = min( dmin, db - blobs[i].w );
	    // }
	    // else // bouncing sphere distance
	    // {
    	// 	dmin = min( dmin, db - blobs[i].w );
    	// }
	}
    // float d = dmin + 0.02;
	
	// if( m>0.5 )
	// {
	// 	float th = 0.03;
	// 	d = h*(th-p);
	// }
	
	return dmin;
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


const float precis = 0.0001;

vec2 intersect( in vec3 ro, in vec3 rd )
{
	float maxd = 10.0;
    float h = precis*2.0;
    float t = 0.0;
    float m = 1.0;
    for( int i=0; i<75; i++ )
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

void main()
{
    //-----------------------------------------------------
    // input
    //-----------------------------------------------------

	vec2 q = gl_FragCoord.xy / iResolution.xy;

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
    for( int i=0; i<numballs; i++ )
    {
        float h = float(i)/8.0;
        blobs[i].xyz = 2.0*sin( 6.2831*hash3(h*1.17) + hash3(h*13.7)*time ) / 10;
        blobs[i].w = (1.7 + 0.9*sin(6.28*hash1(h*23.13))) / 10;
    }

    //-----------------------------------------------------
    // Initialize Camera
    //-----------------------------------------------------

    // image plane		
    vec2 p = (2.0*(gl_FragCoord.xy)-iResolution.xy) / iResolution.y;

    // camera matrix
    vec3 ww = vec3(0.0,0.0,-2.4);
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    // create view ray
    vec3 rd = normalize( vec3( invView * vec4(p.x*uu + p.y*vv + ww, 0) ));
    
    //-----------------------------------------------------
    // render
    //-----------------------------------------------------

    // background
    // vec3 col = pow( texture( iChannel0, rd ).xyz, vec3(2.2) );
    vec3 col = vec3(0);
    
    vec3 ro = camPos;
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
        col = mate * lin;
        // col = mate;
    }
    // gamma
    col = pow( clamp(col,0.0,1.0), vec3(0.45) );
    tot += col;
    
	//-----------------------------------------------------
	// postprocessing
    //-----------------------------------------------------

	// vignetting
    tot *= 0.5 + 0.5*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );
    if(tot.rgb == vec3(0))
        // float x = 0;
        discard;
    FragColor = vec4(tot, 1.0);
}