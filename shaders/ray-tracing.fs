#version 330 core

// ------------------------------------------------------------------------------------------------
// DATA STRUCTURES
// ------------------------------------------------------------------------------------------------

struct Material {
    vec3 ambient;       // propriété ambiente du materiel
    vec3 diffuse;       // propriété diffuse du materiel
    vec3 specular;      // propriété speculaire du materiel
    float shininess;    // proriété brillante du materiel
};

struct Sphere {
    vec3 position;      // position de la sphere
    float radius;       // rayon de la sphere

    Material material;  // materiel de la sphere
};

struct Light {
    vec3 position;      // position de la lumière

    Material material;  // materiel de la lumière
};

struct Ray {
    vec3 origin;        // origine du rayon
    vec3 direction;     // direction du rayon (normalisée)
};

struct Camera {
    vec3 position;      // position caméra dans le monde
    vec3 forward;       // direction de visée (normalisée)
    vec3 up;            // vecteur up (normalisé)
    float fov;          // champ de vision vertical en radians
};

struct hitRecord {
    bool hit;           // boolean for hit status
    vec3 position;      // position of the hitted point
    vec3 normal;        // normal of the hitted point
    float t;            // t value of the ray at hitted point
};

// ------------------------------------------------------------------------------------------------
// IN/OUT PARAMETERS
// ------------------------------------------------------------------------------------------------

in vec2 UV;
out vec4 FragColor;

// ------------------------------------------------------------------------------------------------
// UNIFORM PARAMETERS
// ------------------------------------------------------------------------------------------------

uniform vec2 u_resolution;
uniform Camera camera;
uniform Sphere sphere;

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

/**
 * Renvoie les coordonées normalisées avec l'origine dans le coin bas gauche
 *
 * @return coordonnées normalisées
 */
vec2 normalizedCoord() {
    vec2 coord = (UV + 1.0f) * 0.5f;
    coord.x *= u_resolution.x/u_resolution.y;
    return coord;
}

/**
 * Renvoie les coordonées normalisées avec l'origine au centre
 *
 * @return coordonnées normalisées
 */
vec2 normalizedCenteredCoord() {
    vec2 coord = UV;
    coord.x *= u_resolution.x/u_resolution.y;
    return coord;
}

/**
 * Construit un rayon partant de la caméra et allant vers le fragment cible
 */
Ray generateRay(Camera cam, vec2 uv) {

    // Repère caméra
    vec3 right = normalize(cross(cam.forward, cam.up));
    vec3 up    = normalize(cross(right, cam.forward));

    // Projection avec FOV
    float scale = tan(cam.fov * 0.5);
    vec3 rayDir = normalize(
        cam.forward +
        uv.x * scale * right +
        uv.y * scale * up
    );

    Ray ray;
    ray.origin = cam.position;
    ray.direction = rayDir;
    return ray;
}

/*
 * Vérifie si le rayon touche la sphère
 */
float rayHitSphere(Ray ray, Sphere sphere) {
    vec3 oc = sphere.position - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float b = -2.0 * dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant) ) / (2.0*a);
    }
}

/**
 * Renvoi la valeur d'un rayon pour une valeur de t tel que
 * R(t) = origin + t * direction
 */
vec3 at(Ray r, float t) {
    return r.origin + r.direction * t;
}

/**
 * Donne la couleur à un point précis de la sphere en fonction de son matériaux et celui de la
 * lumière
 */
vec3 colorAt(Sphere sphere, vec3 normal, vec3 hitPoint, Light light) {
    
    // Diffuse shading
    vec3 lightDir = normalize(hitPoint - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(camera.forward, reflectDir), 0.0), sphere.material.shininess);

    vec3 ambient = sphere.material.ambient * light.material.ambient;
    vec3 diffuse = sphere.material.diffuse * diff * light.material.diffuse;
    vec3 specular = sphere.material.specular * spec * light.material.specular;

    return (ambient + diffuse + specular);
}

/**
 * Renvoie la couleur du background pour obtenir un dégradé type "ciel"
 */
vec3 backgroundColor(Ray ray) {
    vec3 unit_direction = ray.direction;
    float a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
}

/**
 * Calcule la couleur du rayon en fonction de l'élément qu'il rencontre en premier (sphere ou bg)
 */
vec3 rayColor(Ray ray, Sphere sphere, Light light) {
    float t = rayHitSphere(ray, sphere);
    if(t > 0.0f) {
        vec3 hitPoint = at(ray, t);
        vec3 normal = normalize(hitPoint - sphere.position);
        return colorAt(sphere, normal, hitPoint, light);
    }
    else {
        return backgroundColor(ray);
    }
}

// ------------------------------------------------------------------------------------------------
// MAIN PROGRAM
// ------------------------------------------------------------------------------------------------

void main()
{
    // Préparation des coordonnées du fragment
    vec2 coord = normalizedCenteredCoord();

    // Création de caméra pour la vue, de la sphère de test et de la lumière
    // Sphere sphere = Sphere(vec3(0.0f), 0.5f, Material(vec3(0.2, 0.2, 0.2), vec3(0.2, 0.1, 0.2), vec3(1.0), 32.0));
    Light light = Light(vec3(0.0, 5.0, 0.0), Material(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0), 0.0));

    // Création du rayon pour ce fragment
    Ray ray = generateRay(camera, coord);

    // Calcul de la couleur du rayon lancé
    FragColor = vec4(rayColor(ray, sphere, light), 1.0);
} 