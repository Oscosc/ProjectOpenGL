#version 460 core

#define SCENE_SPHERES 5

#define RAY_PER_PIXEL 1
#define MAX_BOUNCES 50
#define MAX_FUZZ_LEVEL 100

// ------------------------------------------------------------------------------------------------
// DATA STRUCTURES
// ------------------------------------------------------------------------------------------------

// INTERVAL ---------------------------------------------------------------------------------------

struct Interval {
    float min;
    float max;
};

// VERTEX -----------------------------------------------------------------------------------------

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
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

// INDEX ------------------------------------------------------------------------------------------

struct Index {
    int vertPos;
    int matPos;
};

// ------------------------------------------------------------------------------------------------
// LAYOUT VALUES (SSBO)
// ------------------------------------------------------------------------------------------------

// Contient l'ensemble des vertex de la scene
layout(std430, binding = 0) buffer VertexBuffer {
    Vertex l_vertices[];
};

// Contient l'ensemble des matériaux des objets de la scène
layout(std430, binding = 1) buffer MaterialBuffer {
    Material l_materials[];
};

// Contient l'ordre des index à lire : 3 index = 1 triangle
layout(std430, binding = 2) buffer IndexBuffer {
    Index l_indexes[];
};

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

    vec3 normal;

    Material material;
};

// From : https://iquilezles.org/articles/intersectors/ (adapted)
// triangle designed by vertices v0, v1 and  v2
bool Triangle_hit(const Triangle tri, const Ray ray, const Interval rayInterval, out HitRecord rec) {
    const float EPS = 1e-6;

    vec3 v0 = tri.a;
    vec3 v1 = tri.b;
    vec3 v2 = tri.c;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    // h = cross(ray.dir, edge2)
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);
    if (abs(a) < EPS) return false; // rayon parallèle ou triangle dégénéré

    float f = 1.0 / a;
    vec3 s = ray.origin - v0;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);
    if (v < 0.0 || (u + v) > 1.0) return false;

    float t = f * dot(edge2, q);
    if (t <= rayInterval.min || t >= rayInterval.max) return false;

    // On a une intersection valide
    rec.t = t;
    rec.position = Ray_at(ray, rec.t);

    // calculer la normale de face (unit)
    vec3 faceNormal = normalize(cross(edge1, edge2));
    HitRecord_setFaceNormal(rec, ray, faceNormal);

    rec.material = tri.material;
    return true;
}

void Index_getVertex(const int indexID, out Vertex vertex, out Material material) {
    Index index = l_indexes[indexID];

    vertex   = l_vertices[index.vertPos];
    material = l_materials[index.matPos];
}

Triangle Index_getTriangle(const int startIndex) {
    Index i0 = l_indexes[startIndex + 0];
    Index i1 = l_indexes[startIndex + 1];
    Index i2 = l_indexes[startIndex + 2];

    Vertex a = l_vertices[i0.vertPos];
    Vertex b = l_vertices[i1.vertPos];
    Vertex c = l_vertices[i2.vertPos];

    Material material = l_materials[i0.matPos];

    vec3 normal = normalize(cross(b.position - a.position, c.position - a.position));

    return Triangle(a.position, b.position, c.position, normal, material);
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
bool World_hit(const Sphere spheres[SCENE_SPHERES], const Ray ray, const Interval rayInterval,
    out HitRecord rec) {

    HitRecord tmpRec;
    bool hitAnything = false;
    float closestSoFar = rayInterval.max;
    Interval interval = rayInterval;
    
    // Boucle sur les sphères de l'environnement
    for(int i = 0; i < SCENE_SPHERES; i++) {
        if(Sphere_hit(spheres[i], ray, interval, tmpRec)) {
            if(tmpRec.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tmpRec.t;
                rec = tmpRec;
                interval.max = closestSoFar;
            }
        }
    }

    // Boucle sur les triangles de l'environnement
    for(int i = 0; i < l_indexes.length(); i+=3) {
        // Creation du triangle
        Triangle tri = Index_getTriangle(i);

        // Intersection rayon-triangle généré
        if(Triangle_hit(tri, ray, interval, tmpRec)) {
            if(tmpRec.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tmpRec.t;
                rec = tmpRec;
                interval.max = closestSoFar;
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
vec4 rayColor(Ray ray, const Sphere spheres[SCENE_SPHERES]) {
    
    Interval rayInterval = Interval(0.001, 100.0);
    
    vec3 accumulatedColor = vec3(1.0);
    vec3 finalColor = vec3(0.0);

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
        HitRecord rec;

        if (World_hit(spheres, ray, rayInterval, rec)) {
            vec3 direction;

            float specularCoeff = Material_getSpecularRatio(rec.material);
            float rand = random(vec3(rec.position.xy, u_time));

            if(rand <= specularCoeff) {
                vec3 fuzz = Material_getFuzz(rec.material) * randomVec3(vec3(rec.position.xy, u_time));
                direction = normalize(reflect(ray.direction, rec.normal) + fuzz);
                accumulatedColor *= rec.material.specular;
            }
            else {
                vec2 sub_rand = rand2D(float(bounce) + dot(rec.position, vec3(12.9898,78.233,45.164)));
                direction = normalize(rec.normal + randomEmisphereVec3(rec.normal, sub_rand));
                accumulatedColor *= rec.material.diffuse;
            }
            
            // ray = Ray(rec.position, direction);
            vec3 newDir = normalize(direction);
            vec3 originOffset = rec.position + rec.normal * 1e-4; // petit epsilon le long de la normale
            ray = Ray(originOffset, newDir);

        } else {
            // finalColor = (bounce <= 0) ? backgroundColor(ray) : accumulatedColor *= 0.7;
            finalColor = accumulatedColor * backgroundColor(ray);
            // DEBUG : finalColor = float(bounce) < 10 ? vec3(float(bounce) / 10) : vec3(1.0, 0.0, 0.0);
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

    // Boucle de lancer de rayon pour un pixel
    vec4 finalColor = vec4(0.0);
    for(int i = 0; i < RAY_PER_PIXEL; i++) {
        // Création du rayon pour ce fragment
        Ray ray = generateRay(camera, coord, u_time); // u_time for randomness

        // Calcul de la couleur du rayon lancé
        finalColor += rayColor(ray, u_spheres);
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