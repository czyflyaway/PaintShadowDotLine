#version 330 core
uniform sampler2D depthbuf;

uniform mat4 uMvpMatrix;
in vec3 frag_pos;
flat in vec3 vert_pos;

out vec4 FragColor;

uniform float scatter_size = 0.2;

bool getShadow(vec3 pos) {
    vec4 p = uMvpMatrix * vec4(pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    // float f = 3.0;
    // float castmask = max(f * (p.x), 0.0) * max(f * (1.0 - p.x), 0.0) * max(f * (p.y), 0.0) * max(f * (1.0 - p.y), 0.0);
    // castmask = min(castmask, 1.0);
    // float d = texture(depthbuf, p.xy).r;
    // return castmask * max(sign(p.z - d - 0.00001), 0.0);

    float closestDepth = texture(depthbuf, p.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = p.z;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    // return shadow;
    return currentDepth > closestDepth;
}
void main()
{

    bool shadow = getShadow(frag_pos);


    vec4 p = uMvpMatrix * vec4(frag_pos, 1.0);
    p.xyz /= p.w;
    p.xyz = p.xyz * 0.5 + 0.5;
    // float f = 3.0;
    // float castmask = max(f * (p.x), 0.0) * max(f * (1.0 - p.x), 0.0) * max(f * (p.y), 0.0) * max(f * (1.0 - p.y), 0.0);
    // castmask = min(castmask, 1.0);
    // float d = texture(depthbuf, p.xy).r;
    // return castmask * max(sign(p.z - d - 0.00001), 0.0);

    float closestDepth = texture(depthbuf, p.xy).r;

    float distance = length(vert_pos - frag_pos);
    // float alpha = 1.0 - shadow + shadow * max(0.0, sign(mod(distance, scatter_size) - scatter_size*0.5));
    // float alpha = 1.0 - shadow;
    float alpha = 0.0f;
    if(shadow)
        alpha = 0.0f;
    else
        alpha = 1.0f;
    // if(shadow - 1.0f < 0.00001)
    //     alpha = 1.0f;
    // if(shadow - 0.0f < 0.00001)
    //     alpha = 0.0f;
    FragColor = vec4(closestDepth, 0.0f, 0.0f, 1.0f);
    // FragColor = vec4(0.0f, alpha, 0.0f, 1.0f);
    // FragColor = vec4(vec2(frag_pos.xy), 0.0f, 1.0f);
    // float shadow = getShadow(frag_pos);
    // float distance = length(vert_pos - frag_pos);

    // // float distance = 1.0f;
    // float alpha = 1.0 - shadow + shadow * max(0.0, sign(mod(distance, scatter_size) - scatter_size*0.5));
    // if(sign(mod(distance, scatter_size) - scatter_size*0.5) > 0)
    //     FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // else if(sign(mod(distance, scatter_size) - scatter_size*0.5) < 0)
    //     FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    // else
    //     FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    // // FragColor = vec4(0.0f, shadow, 0.0f, 1.0f);
}
