#version 330 core
out vec4 FragColor;
in vec2 TexCoords; // Coordonnées normalisées [-1, 1] pour chaque fragment

uniform vec2 screenSize;

void main()
{
    // Épaisseur du crosshair (ajuste si nécessaire)
    float thickness = 0.002;
    float xRatio = screenSize.y / screenSize.x;
    float size = 0.02;

    // Vérifie si le fragment est sur la ligne horizontale ou verticale du crosshair
    if ((abs(TexCoords.x) < thickness || abs(TexCoords.y) < thickness)
        && abs(TexCoords.x) < (size * xRatio) && abs(TexCoords.y) < size) {
            FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    else
        discard; // Ne rien dessiner en dehors du crosshair
}