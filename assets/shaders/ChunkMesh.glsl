#ifdef VERTEX_SHADER

layout(location = 0) in vec3  aPos;
layout(location = 1) in vec2  aBaseUV;
layout(location = 2) in vec2  aOverlayUV;
layout(location = 3) in vec3  aNormal;
layout(location = 4) in vec3  aBlockOrigin;
layout(location = 5) in vec3  aTint;
layout(location = 6) in float aUseOverlay;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out vec3  fTint;
out vec3  fNormal;
out vec2  fBaseUV;
out vec2  fOverlayUV;
out vec3  fWorldPos;
out vec3  fBlockOrigin;
out float fUseOverlay;

void main() {
    fTint        = aTint;
    fNormal      = aNormal;
    fBaseUV      = aBaseUV;
    fOverlayUV   = aOverlayUV;
    fWorldPos    = aPos;
    fUseOverlay  = aUseOverlay;
    fBlockOrigin = aBlockOrigin;
    gl_Position  = projection * view * vec4(aPos, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

in vec2  fBaseUV;
in vec2  fOverlayUV;
in vec3  fNormal;
in vec3  fWorldPos;
in vec3  fBlockOrigin;
in vec3  fTint;
in float fUseOverlay;

uniform sampler2D u_atlas;   // unit 2

// Directional light
uniform vec3 u_lightDir;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;

uniform bool  u_isSelected;
uniform ivec3 u_selectedBlock;

out vec4 FragColor;

void main() {
    // Sample texture by block type
    vec4 base = texture(u_atlas, fBaseUV);

    if(fUseOverlay > 0.5f)
    {
        vec4 overlay = texture(u_atlas, fOverlayUV);
        vec3 tinted = overlay.rgb * fTint;
        base.rgb = mix(base.rgb, tinted, overlay.a);
    }
    else
    {
        base.rgb *= fTint;
    }
    
    // Simple directional light
    float diff    = max(dot(normalize(fNormal), -normalize(u_lightDir)), 0.0);
    vec3  lighting = u_ambient + u_diffuse * diff;

    // Highlight selected block
    if(u_isSelected)
        if(ivec3(fBlockOrigin) == u_selectedBlock)
            base.rgb /= 0.75f;

    FragColor = vec4(base.rgb * lighting, base.a);
}

#endif