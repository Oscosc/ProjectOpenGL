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

#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <vector>
#include <iostream>
#include "ScalableElement.hpp"

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

    /**
     * @brief Constructeur par défaut.
     */
    AppContext();

    /**
     * @brief Ajoute un objet à la liste des objets du contexte.
     * Si c'est le premier élément a être ajouté, initialise le pointeur vers l'objet actif à 0.
     * 
     * @param new_object Objet héritant de l'interface ScalableElement.
     */
    void addObject(ScalableElement* new_object);

    /**
     * @brief Retourne la référence à l'objet actif dans le contexte.
     */
    ScalableElement* getActiveObject();

private:
    std::vector<ScalableElement*> m_objects;
    size_t m_activeObjectIndex;
};

#endif //APP_CONTEXT_HPP