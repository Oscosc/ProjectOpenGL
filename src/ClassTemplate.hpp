#include <glm/glm.hpp>
#include <vector>
#include <camera.hpp>

// Abstract
class Object {
    glm::vec3 origin; // Point d'origine d'un objet
    glm::vec3 color;  // Couleur de l'objet

    unsigned int VAO; // VAO pour l'affichage de l'objet
    unsigned int VBO; // VBO pour l'affichage de l'objet
    unsigned int EBO; // EBO pour l'affichage de l'objet

    void draw();           // Un objet doit pouvoir être dessiné
    void updateVertices(); // On doit pouvoir mettre à jour les points d'un objet
};

// Abstract
class ScalableObject : public Object {
    unsigned int scalableValue; // valeur modifiable de l'objet

    void next();     // On doit pouvoir augmenter la valeur modifiable
    void previous(); // On doit pouvoir diminuer la valeur modifiable
};

// Abstract
class WireFrameObject : public ScalableObject {
    float ambient;        // Couleur ambiente
    unsigned int wireEBO; // EBO pour le wireframe
};


class Sphere : public WireFrameObject {

};


class Surface : public WireFrameObject {

};


class Curve : public ScalableObject {

};


class Ray : public Object {

};

// Static Functions
class BaseFunctions {

};


class AppContext {
    // Gestion des objets dans la scène
    std::vector<Object> objectList;
        void addObject();
        void begin();
        void end();
        void clearRays();
        void getObject();
    
    unsigned int activeObject;
        void getActiveObject();
        void nextObject();
        void previousObject();

    
    // Gestion des informations de rendu de la scène
    glm::vec3 backgroundColor;
        void get();
        void set();

    glm::vec3 lightColor;
        void get();
        void set();

    glm::vec3 lightPosition;
        void get();
        void set();


    // Gestion de l'espace de vue utilisateur
    glm::mat4 projection;
        void get();
        void set();

    glm::mat4 view;
        void get();
        void set();

    Camera camera;
        void get();


    // Gestion du curseur
    float cursorX;
        void get();
        void set();

    float cursorY;
        void get();
        void set();

    bool mouseActive;
        void is();
        void switch_();
        
    bool firstMouse;


    // Gestion des déplacements
    float deltaTime;
    float lastFrame;
};