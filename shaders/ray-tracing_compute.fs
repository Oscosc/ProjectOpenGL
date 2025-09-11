#version 330 core

#define SCENE_SPHERES 5
#define SCENE_TRIANGLES 1

#define RAY_PER_PIXEL 1
#define MAX_BOUNCES 100
#define MAX_FUZZ_LEVEL 100

// ------------------------------------------------------------------------------------------------
// DATA STRUCTURES
// ------------------------------------------------------------------------------------------------

// INTERVAL ---------------------------------------------------------------------------------------

struct Interval {
    float min;
    float max;
};

// MATERIAL ---------------------------------------------------------------------------------------

struct Material {
    vec3 ambient;       // propriété ambiente du materiel   -> inutilisé
    vec3 diffuse;       // propriété diffuse du materiel    -> normalisée vers albedo
    vec3 specular;      // propriété speculaire du materiel -> normalisée vers metalness (reflet)

    float shininess;    // proriété brillante du materiel   -> normalisée vers fuzz (granularité)
};

// Donne la part de rayons spéculaires (reflets) du matériel
float Material_getSpecularRatio(const Material material) {
    return length(material.specular) / (length(material.specular) + length(material.diffuse));
}

// Donne le niveau de granularité du matériel
float Material_getFuzz(const Material material) {
    return material.shininess / MAX_FUZZ_LEVEL;
}

// RAY --------------------------------------------------------------------------------------------

struct Ray {
    vec3 origin;        // origine du rayon
    vec3 direction;     // direction du rayon (normalisée)
};

// Donne le point qui correspond à la direction du rayon multipliée par un pas de t
vec3 Ray_at(const Ray r, const float t) {
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

// Définit la normale du point touché sur la sphere
void HitRecord_setFaceNormal(inout HitRecord rec, const Ray r, const vec3 outwardNormal) {
    rec.frontFace = dot(r.direction, outwardNormal) < 0;
    rec.normal = rec.frontFace ? outwardNormal : -outwardNormal;
}

// TRIANGLE ---------------------------------------------------------------------------------------

struct Triangle {
    vec3 a;
    vec3 b;
    vec3 c;

    Material material;
};

// From : https://iquilezles.org/articles/intersectors/ (adapted)
// triangle degined by vertices v0, v1 and  v2
bool Triangle_hit(const Triangle triangle, const Ray ray, const Interval rayInterval, out HitRecord rec) {

    // Calcul de la position de l'intersection
    vec3 ba = triangle.b - triangle.a;
    vec3 ca = triangle.c - triangle.a;
    vec3 oa = ray.origin - triangle.a;

    vec3  n = cross(ba, ca);
    vec3  q = cross(oa, ray.direction);
    float d = 1.0 / dot(ray.direction, n);

    float u = d*dot(-q, ca);
    float v = d*dot( q, ba);
    float t = d*dot(-n, oa);

    // Pas d'intersection
    if(u < 0.0 || v < 0.0 || (u+v) > 1.0) return false;
    if(t <= rayInterval.min || t >= rayInterval.max) return false;

    // Setup du record
    rec.t = t;
    rec.position = Ray_at(ray, rec.t);
    HitRecord_setFaceNormal(rec, ray, normalize(n));
    rec.material = triangle.material;

    return true;
}

// SPHERE -----------------------------------------------------------------------------------------

struct Sphere {
    vec3 position;      // position de la sphere
    float radius;       // rayon de la sphere

    Material material;  // materiel de la sphere
};

// Calcule l'intersection rayon-sphère et sauvegarde les données en cas d'intersection dans un rec
bool Sphere_hit(const Sphere sphere, const Ray ray, const Interval rayInterval, out HitRecord rec) {
    
    // Calcul du discriminant
    vec3 oc = sphere.position - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius*sphere.radius;

    // Discriminant < 0 : pas d'intersection
    float discriminant = h*h - a*c;
    if(discriminant < 0)
        return false;
    
    float sqrtd = sqrt(discriminant);

    // Vérifie que la racine soit bien dans l'interval d'action du rayon
    float root = (h - sqrtd) / a;
    if (root <= rayInterval.min || rayInterval.max <= root) {
        root = (h + sqrtd) / a;
        if (root <= rayInterval.min || rayInterval.max <= root)
            return false;
    }

    // Intersection correcte : setup du record
    rec.t = root;
    rec.position = Ray_at(ray, rec.t);
    vec3 outwardNormal = (rec.position - sphere.position) / sphere.radius;
    HitRecord_setFaceNormal(rec, ray, outwardNormal);
    rec.material = sphere.material;

    return true;
}

// Calcule une intersection entre un rayon et l'environnement (appelle l'intersection avec chaque
// objet)
bool World_hit(const Sphere spheres[SCENE_SPHERES], const Triangle triangles[SCENE_TRIANGLES],
    const Ray ray, const Interval rayInterval, out HitRecord rec) {

    HitRecord tmpRec;
    bool hitAnything = false;
    float closestSoFar = rayInterval.max;
    
    // Boucle sur les sphères de l'environnement
    for(int i = 0; i < SCENE_SPHERES; i++) {
        if(Sphere_hit(spheres[i], ray, rayInterval, tmpRec)) {
            if(tmpRec.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tmpRec.t;
                rec = tmpRec;
            }
        }
    }

    // Boucle sur les triangles de l'environnement
    for(int i = 0; i < SCENE_TRIANGLES; i++) {
        if(Triangle_hit(triangles[i], ray, rayInterval, tmpRec)) {
            if(tmpRec.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tmpRec.t;
                rec = tmpRec;
            }
        }
    }

    return hitAnything;
}

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
uniform Sphere u_spheres[SCENE_SPHERES];
Triangle u_triangles[SCENE_TRIANGLES]; // TODO : set to uniform

// ------------------------------------------------------------------------------------------------
// IN/OUT PARAMETERS
// ------------------------------------------------------------------------------------------------

in vec2 TexCoords; // Défini sur [ 0 ;  1] pour le mapping des textures
in vec2 RayUV;     // Défini sur [-1 ; +1]
out vec4 FragColor;

// VARIOUS FUNCTIONS ------------------------------------------------------------------------------

// Calcule un nombre aléatoire entre 0 et 1 à partir d'une seed vec3
float random(const vec3 coord) {
    return fract(sin(dot(coord, vec3(64.25375463, 23.27536534, 86.29678483))) * 59482.7542);
}

// Calcule un vecteur normalisé aléatoire à partir d'une seed vec3
vec3 randomVec3(const vec3 coord) {
    return normalize(vec3(random(coord.xyz), random(coord.yzx), random(coord.zxy)));
}

// Calcule un vecteur normalisé aléatoire dans l'émismphere de la normale
vec3 randomEmisphereVec3(const vec3 normal, const vec2 rand) {
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

vec2 rand2D(const float seed) {
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
Ray generateRay(const Camera cam, const vec2 uv, const float seed) {

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

// Couleur du ciel (simulé pour avoir un rendu plus sympathique)
vec3 backgroundColor(const Ray ray) {

    vec3 unit_direction = ray.direction;
    float a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
}

vec3 gammaCorrection(const vec3 color) {
    return pow(color, vec3(1.0/2.2));
}

/**
 * Calcule la couleur du rayon en fonction de l'élément qu'il rencontre en premier (sphere ou bg)
 */
vec4 rayColor(Ray ray, const Sphere spheres[SCENE_SPHERES], const Triangle triangles[SCENE_TRIANGLES]) {
    
    Interval rayInterval = Interval(0.001, 100.0);
    
    vec3 accumulatedColor = vec3(1.0);
    vec3 finalColor = vec3(0.0);

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
        HitRecord rec;

        if (World_hit(spheres, triangles, ray, rayInterval, rec)) {
            vec3 direction;

            float specularCoeff = Material_getSpecularRatio(rec.material);
            float rand = random(vec3(rec.position.xy, u_time));

            if(rand <= specularCoeff) {
                vec3 fuzz = Material_getFuzz(rec.material) * randomVec3(vec3(rec.position.xy, u_time));
                direction = reflect(ray.direction, rec.normal) + fuzz;
                accumulatedColor *= rec.material.specular;
            }
            else {
                vec2 sub_rand = rand2D(float(bounce) + dot(rec.position, vec3(12.9898,78.233,45.164)));
                direction = rec.normal + randomEmisphereVec3(rec.normal, sub_rand);
                accumulatedColor *= rec.material.diffuse;
            }
            
            ray = Ray(rec.position, direction);

        } else {
            // finalColor = (bounce <= 0) ? backgroundColor(ray) : accumulatedColor *= 0.7;
            finalColor = accumulatedColor * backgroundColor(ray);
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

    // TEMPORARY : création du triangle
    Material matTri = Material(
        vec3(0.0),
        vec3(1.0, 0.0, 0.0),
        vec3(0.0),
        0.0
    );
    Triangle tri = Triangle(
        vec3(0.0, 5.0, 3.0),
        vec3(2.0, 3.0, 0.0),
        vec3(-2.0, 1.0, 0.0),
        matTri
    );
    u_triangles[0] = tri;

    // Boucle de lancer de rayon pour un pixel
    vec4 finalColor = vec4(0.0);
    for(int i = 0; i < RAY_PER_PIXEL; i++) {
        // Création du rayon pour ce fragment
        Ray ray = generateRay(camera, coord, u_time); // u_time for randomness

        // Calcul de la couleur du rayon lancé
        finalColor += rayColor(ray, u_spheres, u_triangles);
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