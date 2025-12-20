#version 130

uniform sampler2D texture;
uniform float saturation; // 0.0 = grayscale, 1.0 = full color

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    
    // Calculate grayscale using luminance weights
    float gray = dot(pixel.rgb, vec3(0.299, 0.587, 0.114));
    
    // Mix between grayscale and original color based on saturation
    vec3 result = mix(vec3(gray), pixel.rgb, saturation);
    
    gl_FragColor = vec4(result, pixel.a) * gl_Color;
}
