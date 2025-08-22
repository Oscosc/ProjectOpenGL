#version 330 core

out vec4 FragColor;

in vec2 uvCoord;

const float nb_cell = 100.;
const float nb_subcell = 1000.;

const float cell_thk = 0.0005;
const float subcell_thk = 0.001;

const vec3 bg_color = vec3(0.2, 0.2, 0.2);
const vec3 cell_color = vec3(0.70, 0.75, 0.75);
const vec3 subcell_color = vec3(0.5, 0.5, 0.5);

bool in_border(vec2 coord, float thk) {
    vec2 d = fwidth(coord);

    return ( coord.x < (thk + d.x) * 0.5
          || coord.y < (thk + d.y) * 0.5
          || coord.x > (1. - (thk + d.x) * 0.5)
          || coord.y > (1. - (thk + d.y) * 0.5)
    );
}

void main() {
    vec2 coord = uvCoord;
    
    vec2 cell_coord = mod(coord, 1./nb_cell) * nb_cell;
    vec2 subcell_coord = mod(coord, 1./nb_subcell) * nb_subcell;
	
    vec3 color = bg_color;
    if(in_border(cell_coord, cell_thk)) color = cell_color;
    else if(in_border(subcell_coord, subcell_thk)) color = subcell_color;

    // float shading = pow(length(coord - vec2(0.5)), 0.5) / 0.5;
    // if(length(coord - vec2(0.5)) > 0.05) shading = 1.0;
    // color = color * (1 - shading) + bg_color * shading;

    FragColor = vec4(color, 1.0);
}