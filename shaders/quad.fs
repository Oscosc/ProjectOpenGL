#version 460 core
out vec4 FragColor;
in vec2 TexCoords; // Coordonnées normalisées [-1, 1] pour chaque fragment

uniform vec2 screenSize;

void main()
{
    float xRatio = screenSize.y / screenSize.x;

    vec2 thickness = vec2(0.002 * xRatio, 0.002);
    vec2 size = vec2(0.02 * xRatio, 0.02);
    
    // Vérifie si le fragment est sur la ligne horizontale ou verticale du crosshair
    if ((abs(TexCoords.x) < thickness.x || abs(TexCoords.y) < thickness.y)
        && abs(TexCoords.x) < (size.x) && abs(TexCoords.y) < size.y) {
            FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    else
        discard; // Ne rien dessiner en dehors du crosshair
}