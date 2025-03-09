#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AppContext.hpp"
#include "Intersections.hpp"

/**
 * @brief Frame-buffer size callback.
 * 
 * Fonction de callback pour que l'utilisateur puisse redimensionner dynamiquement la fenêtre.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param width Nouvelle largeur de la fenêtre.
 * @param height Nouvelle hauteur de la fenêtre.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Mouse callback.
 * 
 * Fonction de callback pour traiter les mouvements du curseur et déplacer la vue dans la scène 3D
 * en fonction de ce mouvement.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param xpos Nouvelle position X du curseur.
 * @param ypos Nouvelle position Y du curseur.
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Scroll callback.
 * 
 * Fonction de callback pour traiter le scroll de l'utilisateur et zoomer/dezoomer la camera.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param xoffset Mouvement en X du scroll.
 * @param yoffset Mouvement en Y du scroll.
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/**
 * @brief Key callback.
 * 
 * Fonction de callback pour les touches du clavier. Les touches du clavier actuellement traitées
 * sont :
 * - Flèche haut (comportement spécifique aux courbes de Bézier)
 * - Flèche bas (comportement spécifique aux courbes de Bézier)
 * - Tab (bascule du mode "curseur" au mode "souris")
 * - M (comportement spécifique aux courbes de Bézier)
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param key Identifiant de la touche qui déclenche le callback.
 * @param scancode Scancode de la touche qui déclenche le callback.
 * @param action Type d'action effectuée sur la touche (pressée, relachée, etc).
 * @param mods Eventuels modifications sur la touche (si ALT est maintenu avec par exemple)
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

/**
 * @brief Traite les inputs "longs" du clavier.
 * 
 * Traite les entrées claviers de type ZQSD qui doivent ête effectuées à chaque frame pour donner
 * un effet "lisse" dans les déplacements par exemple.
 * @param window Fenêtre à laquelle on veut assigner le traitement des saisies clavier.
 */
void processInput(GLFWwindow *window);

#endif // CALLBACKS_HPP