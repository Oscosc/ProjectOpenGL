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
    void addObject(uniqueObject new_object);

    /**
     * @brief Retourne la référence à l'objet actif dans le contexte. (Cette fonction effectue un
     * dynamic_cast vers le type ScalableElement car les données initiales sont stockées comme des
     * objets) 
     */
    ScalableElement* getActiveObject();
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

    unsigned int size() const;

    void removeRays();

private:
    uniqueObjectsList m_objects;
    size_t m_activeObjectIndex;
};

#endif //APP_CONTEXT_HPP