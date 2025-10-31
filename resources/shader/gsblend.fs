#version 330

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// NOTE: Add here your custom variables
uniform float intensity;
uniform int blendMode;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture2D(texture0, fragTexCoord);
    
    texelColor *= fragColor;
    
    // fade out/fade in blending
    if(blendMode == 0) {

    } else if(blendMode == 1) {

    } else if(blendMode == 3) { // darken
        texelColor.r -= texelColor.r*intensity;
        texelColor.g -= texelColor.g*intensity;
        texelColor.b -= texelColor.b*intensity;
    } else if(blendMode == 2) { // lighten
        texelColor.r += (1.0-texelColor.r)*intensity;
        texelColor.g += (1.0-texelColor.g)*intensity;
        texelColor.b += (1.0-texelColor.b)*intensity;
    }
    gl_FragColor = texelColor*colDiffuse;
}