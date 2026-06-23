#version 330
uniform float m_time;
uniform sampler2D spct;
uniform vec2 m_resolution;
uniform float zoom;
uniform float ampRatio;
uniform float sampler_0;
uniform float sampler_1;
out vec4 FragColor;



mat2 rotate(float a){
    return mat2(cos(a),-sin(a),
                sin(a),cos(a));
}



#define TURB_NUM 7.0
#define TURB_AMP .7
#define TURB_SPEED .7
#define TURB_FREQ .7
#define TURB_EXP 1.4

float freq = TURB_FREQ;
mat3 rotx = mat3(1.,0.,0.,
                0.,0.6, -0.8, 
                0.,0.8, 0.6);
mat3 rotz = mat3(0.8, -0.6,0., 
                 0.6, 0.8,0.,
                 0.,0.,1.);
mat3 roty = mat3(0.8, 0.,-0.6,
                0.,1.,0.,
                 0.6, 0.,0.8);
mat2 rot=mat2(.6,-.8,.8,.6);

vec3 torsion(vec3 pos){
    mat3 E=mat3(1.);
    float WAVE_SPEED=TURB_SPEED;
    float WAVE_AMP=TURB_AMP;
    E*=rotx;
    E*=rotz;
    for(int i=0;i<TURB_NUM;i++){
        float phase=freq * (pos*E).y + WAVE_SPEED*m_time/500.;
        pos+=WAVE_AMP * E[0] * sin(phase*max(min(log2(sampler_0),15.),.3)) / freq;
        E*=rotx;
        E*=roty;
        freq*=TURB_EXP;
    }
    freq=TURB_FREQ;

    return pos;
}



vec3 hash33(vec3 p3){
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);
}
vec2 hash1to2(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}
vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}


#define DENSITY 1.6
#define PASSTHROUGH 0.1
float sphere(vec3 p){
    return abs(length(p)-(5.+tanh(sampler_0*0.05)))+PASSTHROUGH;
}


vec2 glitch(vec2 uv){
    vec2 cell=vec2(floor(5.*(uv+vec2(1.,sin(uv.y+m_time)*0.2))*vec2(1.,2.*sin(m_time))).y);
    vec2 offset=2.*hash22(cell+sin(m_time))-1.;
    return uv+offset*smoothstep(50.,100.,sampler_1)*.4;
}

vec2 glitchOffset(vec2 uv,float seed){
    vec2 cell=vec2(floor(5.*(uv+vec2(1.,sin(uv.y+m_time)*0.2))*vec2(1.,seed*sin(m_time))).y);
    vec2 offset=2.*hash22(cell+sin(m_time))-1.;
    return offset*smoothstep(50.,100.,sampler_1)*.1;
}

#define BRIGHTNESS 0.05
#define STEPS 10
vec3 Image(vec2 u){
    vec3 col=vec3(0.);
    u=glitch(glitch(u.yx)).yx;
    vec3 dir = normalize(vec3(u+u,1.));
    vec3 pos = vec3(0.,-1.,-14);
    for(float i=0.;i<STEPS;i++){
        float vol=sphere(torsion(pos));
        pos+=dir*(vol);

        col+=vec3(.3,.9,1.)/vol;
    }
    return col;
}


void main(){
    vec3 Col=vec3(0.);
    vec2 u=(gl_FragCoord.xy-.5*m_resolution)/m_resolution.y;
    Col+=vec3(Image(u+glitchOffset(u,0.)).r,0.,0.);
    Col+=vec3(0.,Image(u+glitchOffset(u,.3)).g,0.);
    Col+=vec3(0.,0.,Image(u+glitchOffset(u,.6)).b);
    // Col.a=1.;

    Col=tanh(BRIGHTNESS*Col);

    FragColor=vec4(Col,1.);

}