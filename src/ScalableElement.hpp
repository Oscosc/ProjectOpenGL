#ifndef SCALABLE_ELEMENT_HPP
#define SCALABLE_ELEMENT_HPP

/**
 * @file ScalableElement.hpp
 * @brief Définition de la classe abstraite/interface ScalableElement.
 * 
 * Ce fichier contient la définition d'un objet modifiable dynamiquement au travers des fonctions
 * de callback de GLFW.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

 
/**
 * @class ScalableElement
 * @brief Classe abstraite/interface pour les éléments qui doivent être modifiables dynamiquement
 * via des fonctions de callback.
 * 
 * Cette classe abstraite est principalement vouée a être héritée par les classes dérivées qui
 * auront leurs objets enregistrés dans le contexte de l'application (de type AppContext) puisque
 * c'est le contexte qui pourra être pointé dans les fonctions de callback.
 */
class ScalableElement
{
public:

    /**
     * @brief Fonction à redéfinir pour chaque classe dérivée.
     * 
     * Met à jour l'élément avec son état suivant.
     */
    virtual void next() = 0;

    /**
     * @brief Fonction à redéfinir pour chaque classe dérivée.
     * 
     * Met à jour l'élément avec son état précédent.
     */
    virtual void previous() = 0;

    /**
     * @brief Fonction qui peut être redéfinie pour chaque classe dérivée.
     * 
     * Par défaut : Fait une bascule simple de l'attribut booléen altModeOn.
     */
    virtual void switchMode() {altModeOn = !altModeOn;}
    
    /**
     * @brief Getter pour l'attribut altModeOn.
     */
    bool isAltModeOn() {return altModeOn;}

protected:

    bool altModeOn = false;
};

#endif //SCALABLE_ELEMENT_HPP