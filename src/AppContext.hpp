#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

/**
 * @file AppContext.hpp
 * @brief Définition de la classe AppContext.
 * 
 * Ce fichier contient la définition du contexe de l'application.
 * Le contexte est relié à OpenGL en utilisant le pointeur utilisateur.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include "ScalableElement.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "../includes/camera.hpp"

#define STANDARD_DISPLAY_MODE 0
#define NORMAL_DISPLAY_MODE 1
#define UV_DISPLAY_MODE 2

/**
 * @class AppContext
 * @brief Objet englobant les éléments du contexte de la fenetre.
 * 
 * En particulier ici, le contexte est utilisé pour modifier des éléments dans les fonctions de
 * callback. Le contexte contient donc une liste de ScalableElement qui sont des objets modifiables
 * génériquement.
 */
class AppContext
{
public:
    using uniqueObjectsList = std::vector<std::unique_ptr<Object>>;
    using uniqueObject = std::unique_ptr<Object>;

    const unsigned int SCR_WIDTH;
    const unsigned int SCR_HEIGHT;

    /**
     * @brief Constructeur par défaut.
     */
    AppContext(unsigned int screen_width, unsigned int screen_height, glm::vec3 backgroundColor,
        glm::vec3 lightColor);

    /**
     * @brief Ajoute un objet à la liste des objets du contexte.
     * Si c'est le premier élément a être ajouté, initialise le pointeur vers l'objet actif à 0.
     * 
     * @param new_object Objet héritant de l'interface ScalableElement.
     */
    void addObject(uniqueObject new_object);

    /**
     * @brief Retourne la référence à l'objet actif dans le contexte. (Cette fonction effectue un
     * dynamic_cast vers le type ScalableElement car les données initiales sont stockées comme des
     * objets) 
     */
    ScalableElement* getActiveAsScalable();

    /**
     * @brief Retourne la référence à l'objet actif dans le contexte.
     */
    Object* getActiveAsObject();


    /**
     * @brief Fonction begin() pour permettre d'itérer sur AppContext.
     * 
     * En particulier ici, on itère sur les éléments du contexte de type Object (car on utilise
     * cette boucle pour l'affichage OpenGL).
     */
    uniqueObjectsList::iterator begin();
    uniqueObjectsList::const_iterator begin() const;

    /**
     * @brief Fonction end() pour permettre d'itérer sur AppContext.
     * 
     * En particulier ici, on itère sur les éléments du contexte de type Object (car on utilise
     * cette boucle pour l'affichage OpenGL)
     */
    uniqueObjectsList::iterator end();
    uniqueObjectsList::const_iterator end() const;

    /**
     * @brief Passe l'objet actif dans la scène à l'objet suivant dans la liste des objets du
     * contexte (effectue une boucle sur les objets).
     */
    void nextObject();

    /**
     * @brief Passe l'objet actif dans la scène à l'objet précedent dans la liste des objets du
     * contexte (effectue une boucle sur les objets).
     */
    void previousObject();
    
    /**
     * @brief Retourne la taille du contexte, ici le nombre d'éléments de type Object dans la
     * scène.
     */
    unsigned int size() const;

    /**
     * @brief Supprime tous les rayons qui ont été lancés dans la scène (et leur réflexion s'ils
     * en ont une).
     */
    void clearRays();
    
    /**
     * @brief Retourne un pointeur vers l'objet d'index égal à celui passé en paramètre. S'il n y a
     * pas d'objet avec cet index, c'est un nullptr qui est renvoyé.
     * @param index index de l'objet que l'on veut récupérer.
     */
    Object* getObject(size_t index);

    /**
     * @brief Retourne la couleur de fond de la scène.
     */
    glm::vec3 getBackgroundColor();

    /**
     * @brief Retourne la couleur de la lumière qui éclaire la scène.
     */
    glm::vec3 getLightColor();

    unsigned int getDisplayMode() const;
    void setDisplayMode(unsigned int value);

    void drawContext(Shader shader);
    
    /**
     * @brief Retourne la view matrix de la scène.
     */
    glm::mat4 getView();

    /**
     * @brief Définit la view matrix de la scène.
     * @param view nouvelle view matrix pour la scène
     */
    void setView(glm::mat4 view);

    /**
     * @brief Retourne la projection matrix de la scène.
     */
    glm::mat4 getProjection();

    /**
     * @brief Définit la projection matrix de la scène.
     * @param projection nouvelle projection matrix pour la scène
     */
    void setProjection(glm::mat4 projection);

    /**
     * @brief Retourne un pointeur vers la caméra de l'utilisateur.
     */
    Camera* getCamera();

    /**
     * @brief Retourne la valeur de position du curseur en X.
     */
    glm::vec2 getCursor();

    /**
     * @brief Définit la position du curseur en X
     * @param value nouvelle valeur de position du curseur en X
     */
    void setCursor(glm::vec2 value);
    void setCursor(float x, float y);

    /**
     * @brief Retourne true si la souris est visible pour l'utilisateur, false sinon
     */
    bool isMouseActive();

    /**
     * @brief Bascule dans l'état opposé la variable de visibilité de la souris. Attention ! Cette
     * fonction NE MODIFIE PAS les valeurs glfw pour le curseur, cette fonction sers juste à
     * modifier un état.
     */
    void switchMouseActive();

    /**
     * @brief Retourne true si l'utilisateur n'a pas encore changé l'état de sa souris depuis le
     * lancement du programme, false sinon.
     */
    bool isFirstMouse();

    /**
     * @brief Bascule à false la variable firstMouse (à appeler quand l'utilisateur à modifié pour
     * la première fois l'état de la souris).
     */
    void firstMouseDone();

    /**
     * @brief Retourne le temps écoulé depuis la dernière frame.
     */
    float getDeltaTime();

    /**
     * @brief Modifie la temps écoulé depuis la dernière frame.
     * @param value nouvelle valeur de temps écoulé.
     */
    void setDeltaTime(float value);

    /**
     * @brief Récupère la valeur de la dernière frame.
     */
    float getLastFrame();

    /**
     * @brief Modifie la valeur de la dernière frame.
     * @param value nouvelle valeur de dernière frame.
     */
    void setLastFrame(float value);

    /**
     * @brief Renvoie l'index de l'élément actif dans la scène.
     */
    unsigned int getActiveIndex();

private:

    uniqueObjectsList m_objects;
    size_t m_activeObjectIndex;

    glm::vec3 m_backgroundColor;
    glm::vec3 m_lightColor;
    unsigned int m_displayMode;
    
    glm::mat4 m_projection;
    glm::mat4 m_view;
    Camera m_camera;

    glm::vec2 m_cursor;

    bool m_mouseActive = true;
    bool m_firstMouse = true; // To remove in future, used only once

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
};

#endif //APP_CONTEXT_HPP