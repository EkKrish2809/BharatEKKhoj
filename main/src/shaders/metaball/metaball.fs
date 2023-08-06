#version 460 core

uniform vec2      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
// uniform float     iTimeDelta;            // render time (in seconds)
// uniform float     iFrameRate;            // shader frame rate
// uniform int       iFrame;                // shader playback frame
// uniform float     iChannelTime[4];       // channel playback time (in seconds)
// uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
// uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
// uniform samplerXX iChannel0..3;          // input channel. XX = 2D/Cube
// uniform vec4      iDate;                 // (year, month, day, time in seconds)

out vec4 FragColor;

// d1 : sphere sdf 1
// d2 : sphere sdf 2
float opUnion( float d1, float d2 )
{
    return min(d1,d2);
}

// d1 : sphere sdf 1
// d2 : sphere sdf 2
// k : smooth step
float opSmoothUnion( float d1, float d2, float k )
{
    float h = max(k-abs(d1-d2),0.0);
    return min(d1, d2) - h*h*0.25/k;
}


//-------------------------------------------------

float sdSphere( in vec3 p, in float r )
{
    return length(p)-r;
}

//---------------------------------

float map(in vec3 pos)
{
    float d = 1e10;
    
    // Set this to a constant to stop the animation
    float an = sin(iTime);

    // opUnion
    {
        vec3 q = pos - vec3(-3.0,0.0,0.0);
        float d1 = sdSphere( q-vec3(0.0,0.5+0.3*an,0.0), 0.55 );
        float d2 = sdSphere( q-vec3(0.0,0.0,0.0), 0.6 );
        float dt = opUnion(d1,d2);
        d = min( d, dt );
    }
    
    // opSmoothUnion
    {
    vec3 q = pos - vec3(-1.0,0.0,0.0);
    float d1 = sdSphere( q-vec3(0.0,0.5+0.3*an,0.0), 0.55 );
    float d2 = sdSphere( q-vec3(0.0,0.0,0.0), 0.6 );
    float dt = opSmoothUnion(d1,d2, 0.25);
    d = min( d, dt );
    }
    

    // Multiple spheres, opUnion
    for( int i=1; i<5; i++ )
    {
        vec3 q = pos - vec3(1.0,0.0,0.0);
        float d1 = sdSphere( q-vec3(0.0,0.3*pow(float(i-1),1.5) - 1.0,0.0), 0.5 );
        float d2 = sdSphere( q-vec3(0.0,0.3*pow(float(i),1.5) - 1.0,0.0), 0.5 );
        float dt = opUnion(d1,d2);
        d = min( d, dt );
    }
    

    // Multiple spheres, opSmoothUnion
    for( int i=1; i<5; i++ )
    {
        vec3 q = pos - vec3(3.0,0.0,0.0);
        float d1 = sdSphere( q-vec3(0.0,0.3*pow(float(i-1),1.5) - 1.0,0.0), 0.5 );
        float d2 = sdSphere( q-vec3(0.0,0.3*pow(float(i),1.5) - 1.0,0.0), 0.5 );
        float dt = opSmoothUnion(d1,d2, 0.15*float(i));
        d = min( d, dt );
    }
    

    return d;
}

// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal( in vec3 pos )
{
    const float ep = 0.0001;
    vec2 e = vec2(1.0,-1.0)*0.5773;
    return normalize( e.xyy*map( pos + e.xyy*ep ) + 
                      e.yyx*map( pos + e.yyx*ep ) + 
                      e.yxy*map( pos + e.yxy*ep ) + 
                      e.xxx*map( pos + e.xxx*ep ) );
}

// https://iquilezles.org/articles/rmshadows
float calcSoftshadow( in vec3 ro, in vec3 rd, float tmin, float tmax, const float k )
{
    float res = 1.0;
    float t = tmin;
    for( int i=0; i<50; i++ )
    {
        float h = map( ro + rd*t );
        res = min( res, k*h/t );
        t += clamp( h, 0.02, 0.20 );
        if( res<0.005 || t>tmax ) break;
    }
    return clamp( res, 0.0, 1.0 );
}


#define AA 2

void main(void)
{
   vec3 tot = vec3(0.0);
    
    #if AA>1
    for( int m=0; m<AA; m++ )
    for( int n=0; n<AA; n++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
        vec2 p = (-iResolution.xy + 2.0*(gl_FragCoord+o))/iResolution.y;
        #else    
        vec2 p = (-iResolution.xy + 2.0*gl_FragCoord)/iResolution.y;
        #endif
 
        vec3 ro = vec3(0.0,4.0,8.0);
        vec3 rd = normalize(vec3(p-vec2(0.0,1.8),-3.5));

        float t = 7.0;
        for( int i=0; i<64; i++ )
        {
            vec3 p = ro + t*rd;
            float h = map(p);
            if( abs(h)<0.001 || t>11.0 ) break;
            t += h;
        }

        vec3 col = vec3(0.0);

        if( t<11.0 )
        {
            vec3 pos = ro + t*rd;
            vec3 nor = calcNormal(pos);
            vec3  lig = normalize(vec3(1.0,0.8,-0.2));
            float dif = clamp(dot(nor,lig),0.0,1.0);
            float sha = calcSoftshadow( pos, lig, 0.001, 1.0, 16.0 );
            float amb = 0.5 + 0.5*nor.y;
            col = vec3(0.05,0.1,0.15)*amb + 
                  vec3(1.00,0.9,0.80)*dif*sha;
        }

        col = sqrt( col );
        tot += col;
    #if AA>1
    }
    tot /= float(AA*AA);
    #endif

    FragColor = vec4( tot, 1.0 );
}

