#version 330 core

#define SCENE_OBJ 2
#define RAY_PER_PIXEL 10
#define MAX_BOUNCES 10

// ------------------------------------------------------------------------------------------------
// DATA STRUCTURES
// ------------------------------------------------------------------------------------------------

struct Material {
    vec3 ambient;       // propriété ambiente du materiel
    vec3 diffuse;       // propriété diffuse du materiel
    vec3 specular;      // propriété speculaire du materiel
    float shininess;    // proriété brillante du materiel
};

// RAY --------------------------------------------------------------------------------------------

struct Ray {
    vec3 origin;        // origine du rayon
    vec3 direction;     // direction du rayon (normalisée)
};

/**
 * Renvoi la valeur d'un rayon pour une valeur de t tel que
 * R(t) = origin + t * direction
 */
vec3 Ray_at(Ray r, float t) {
    return r.origin + r.direction * t;
}

// HIT RECORD -------------------------------------------------------------------------------------

struct HitRecord {
    vec3 position;      // position of the hitted point
    vec3 normal;        // normal of the hitted point
    float t;            // t value of the ray at hitted point
    bool frontFace;     // boolean to define if sphere is hitted on front

    Material material;  // Material of the hitted surface
};

/**
 * Compute the normal and the side of the face hitted by the ray
 */
void HitRecord_setFaceNormal(inout HitRecord rec, const in Ray r, const in vec3 outwardNormal) {
    rec.frontFace = dot(r.direction, outwardNormal) < 0;
    rec.normal = rec.frontFace ? outwardNormal : -outwardNormal;
}

// SPHERE -----------------------------------------------------------------------------------------

struct Sphere {
    vec3 position;      // position de la sphere
    float radius;       // rayon de la sphere

    Material material;  // materiel de la sphere
};

bool Sphere_hit(in Sphere sphere, const in Ray ray, const in float ray_tMin, const in float ray_tMax, out HitRecord rec) {
    
    // Compute the discriminant
    vec3 oc = sphere.position - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius*sphere.radius;

    float discriminant = h*h - a*c;
    if(discriminant < 0)
        return false;
    
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that's lies in the acceptable range
    float root = (h - sqrtd) / a;
    if (root <= ray_tMin || ray_tMax <= root) {
        root = (h + sqrtd) / a;
        if (root <= ray_tMin || ray_tMax <= root)
            return false;
    }

    // Setup the record
    rec.t = root;
    rec.position = Ray_at(ray, rec.t);
    vec3 outwardNormal = (rec.position - sphere.position) / sphere.radius;
    HitRecord_setFaceNormal(rec, ray, outwardNormal);
    rec.material = sphere.material;

    return true;
}

bool World_hit(in Sphere spheres[SCENE_OBJ], const in Ray ray, const in float ray_tMin, const in float ray_tMax, out HitRecord rec) {
    HitRecord tmpRec;
    bool hitAnything = false;
    float closestSoFar = ray_tMax;

    // Loop over world objects
    for(int i = 0; i < SCENE_OBJ; i++) {
        if(Sphere_hit(spheres[i], ray, ray_tMin, ray_tMax, tmpRec)) {
            if(tmpRec.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tmpRec.t;
                rec = tmpRec;
            }
        }
    }

    return hitAnything;
}

struct Light {
    vec3 position;      // position de la lumière

    Material material;  // materiel de la lumière
};

// CAMERA -----------------------------------------------------------------------------------------

struct Camera {
    vec3 position;      // position caméra dans le monde
    vec3 forward;       // direction de visée (normalisée)
    vec3 up;            // vecteur up (normalisé)
    float fov;          // champ de vision vertical en radians
};

// ------------------------------------------------------------------------------------------------
// UNIFORM PARAMETERS
// ------------------------------------------------------------------------------------------------

uniform vec2 u_resolution;
uniform float u_time;

uniform sampler2D u_previousFrame;
uniform int u_frameCount;

uniform Camera camera;
uniform Sphere spheres[SCENE_OBJ];

// ------------------------------------------------------------------------------------------------
// IN/OUT PARAMETERS
// ------------------------------------------------------------------------------------------------

in vec2 TexCoords; // Défini sur [ 0 ;  1] pour le mapping des textures
in vec2 RayUV;     // Défini sur [-1 ; +1]
out vec4 FragColor;

// VARIOUS FUNCTIONS ------------------------------------------------------------------------------

float random(const vec3 coord) {
    return fract(sin(dot(coord, vec3(64.25375463, 23.27536534, 86.29678483))) * 59482.7542);
}

vec3 randomVec3(const vec3 coord) {
    return normalize(vec3(random(coord.xyz), random(coord.yzx), random(coord.zxy)));
}

vec3 cosineWeightedHemisphere(vec3 normal, vec2 rand) {
    float r = sqrt(rand.x);
    float theta = 2.0 * 3.14159265359 * rand.y;

    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(max(0.0, 1.0 - rand.x));

    vec3 localDir = vec3(x, y, z);

    // même transform en espace monde
    vec3 up = abs(normal.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    return tangent * localDir.x + bitangent * localDir.y + normal * localDir.z;
}

vec2 rand2D(float seed) {
    return vec2(
        random(vec3(seed, u_time, 1.23)),
        random(vec3(seed, u_time, 4.56))
    );
}

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

/**
 * Renvoie les coordonées normalisées avec l'origine au centre
 *
 * @return coordonnées normalisées
 */
vec2 normalizedCenteredCoord() {
    vec2 coord = RayUV;
    coord.x *= u_resolution.x/u_resolution.y;
    return coord;
}

/**
 * Construct a ray coming from the camera and directed to the active pixel
 */
Ray generateRay(Camera cam, vec2 uv, float seed) {

    // Camera
    vec3 right = normalize(cross(cam.forward, cam.up));
    vec3 up    = normalize(cross(right, cam.forward));

    // Antialiasing
    vec3 offset = randomVec3(vec3(uv, seed));
    float xOffset = (offset.x - 0.5) / u_resolution.x;
    float yOffset = (offset.y - 0.5) / u_resolution.y;

    // Projection avec FOV
    float scale = tan(cam.fov * 0.5);
    vec3 rayDir = normalize(
        cam.forward +
        (uv.x + xOffset) * scale * right +
        (uv.y + yOffset) * scale * up
    );

    Ray ray;
    ray.origin = cam.position;
    ray.direction = rayDir;
    return ray;
}

/**
 * Donne la couleur à un point précis de la sphere en fonction de son matériaux et celui de la
 * lumière
 */
vec3 colorAt(const in Material material, const in vec3 normal, const in vec3 hitPoint, const in Light light) {
    
    // Diffuse shading
    vec3 lightDir = normalize(hitPoint - light.position);
    float diff = max(dot(normal, -lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(camera.forward, reflectDir), 0.0), material.shininess);

    vec3 ambient = material.ambient * light.material.ambient;
    vec3 diffuse = material.diffuse * diff * light.material.diffuse;
    vec3 specular = material.specular * spec * light.material.specular;

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
vec4 rayColor(Ray ray, Sphere spheres[SCENE_OBJ], Light light) {
    vec3 accumulatedColor = vec3(1.0);
    vec3 finalColor = vec3(0.0);

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
        HitRecord rec;
        if (World_hit(spheres, ray, 0.001, 100.0, rec)) {
            // scatter direction
            vec2 rand = rand2D(float(bounce) + dot(rec.position, vec3(12.9898,78.233,45.164)));
            vec3 direction = rec.normal + cosineWeightedHemisphere(rec.normal, rand);

            // Atténuation
            accumulatedColor *= colorAt(rec.material, rec.normal, rec.position, light);

            // Mettre à jour le rayon pour le prochain tour
            ray = Ray(rec.position, direction);
        } else {
            // Si pas de hit → couleur background * contribution
            finalColor = accumulatedColor *= backgroundColor(ray);
            break;
        }
    }

    return vec4(finalColor, 1.0);
}

// ------------------------------------------------------------------------------------------------
// MAIN PROGRAM
// ------------------------------------------------------------------------------------------------

void main()
{
    // Préparation des coordonnées du fragment
    vec2 coord = normalizedCenteredCoord();

    // Création de la source de lumière (pas de lien avec l'app : TODO)
    Light light = Light(vec3(0.0, 5.0, 0.0), Material(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0), 0.0));

    // Boucle de lancer de rayon pour un pixel
    vec4 finalColor = vec4(0.0);
    for(int i = 0; i < RAY_PER_PIXEL; i++) {
        // Création du rayon pour ce fragment
        Ray ray = generateRay(camera, coord, u_time); // u_time for randomness

        // Calcul de la couleur du rayon lancé
        finalColor += rayColor(ray, spheres, light);
    }

    // Couleur finale
    vec3 newColor = (finalColor / RAY_PER_PIXEL).xyz; // TODO : repasser en vec4

    // Mélange avec l'image précédente
    vec3 previousColor = texture(u_previousFrame, TexCoords).xyz;
    float alpha = 1.0 / float(u_frameCount);
    vec3 accumulatedColor = (u_frameCount <= 1)
        ? newColor
        : mix(previousColor, newColor, alpha);

    // output
    FragColor = vec4(accumulatedColor, 1.0);
} 