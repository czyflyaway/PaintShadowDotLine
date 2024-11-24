#version 330 core
uniform sampler2D depthbuf;

uniform mat4 uMvpMatrix;

in vec3 frag_pos;
flat in vec3 vert_pos;

out vec4 FragColor;

uniform float scatter_size = 0.2;

float getShadow(vec3 pos) {
    vec4 p = uMvpMatrix * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    float f = 3.0;
    float castmask = max(f * (p.x), 0.0) * max(f * (1.0 - p.x), 0.0) * max(f * (p.y), 0.0) * max(f * (1.0 - p.y), 0.0);
    castmask = min(castmask, 1.0);
    float d = texture(depthbuf, p.xy).r;
//    return castmask * max(sign(p.z - d - 0.00001), 0.0);

    return p.z > d ? 1.0 : 0.0;
//    if(abs(d - p.z) < 0.000001)
//        return 1.0;
//    return 0.0f;
}

void main() {
    float shadow = getShadow(frag_pos);
    float distance = length(vert_pos - frag_pos);
    float alpha = 1.0 - shadow + shadow * max(0.0, sign(mod(distance, scatter_size) - scatter_size*0.5));
    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}
