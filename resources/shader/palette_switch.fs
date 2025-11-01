#version 330

// Input fragment attributes (from fragment shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
//uniform ivec3 palette[MAX_INDEXED_COLORS];
uniform sampler2D palette; // Alternative to ivec3, palette provided as a 256x1 texture

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    int index = (int(texelColor.x*255.0) & 15);
    if(index == 0) finalColor = vec4(0, 0, 0, 0);
    else {
        texelColor.x = (float(index + (fragColor.b*255)*16))/255;
        finalColor = texture(palette, texelColor.xy) * fragColor.a;
    }
}
