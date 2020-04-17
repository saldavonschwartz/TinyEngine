#VERTEX_STAGE

#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 txCoords;
layout (location = 2) in vec2 txOffset; // instanced
layout (location = 3) in mat4 W;        // instanced

out vec2 uvCoords;
flat out vec2 uvOffset;
flat out int id;

void main() {
  id = gl_InstanceID;
  uvCoords = txCoords;
  uvOffset = txOffset;
  gl_Position = W * vec4(position, 1.);
}


#FRAGMENT_STAGE

#version 410 core
in vec2 uvCoords;
flat in vec2 uvOffset;
flat in int id;

out vec4 fragColor;

uniform vec3 tint[50];
uniform sampler2D sprite;

void main() {
  vec4 color = texture(sprite, (uvCoords+uvOffset)/2.);
  
  if (color.a < 0.1) {
    discard;
  }
  
  fragColor = vec4(tint[id], 1.) * color;
}
