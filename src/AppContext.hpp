#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <vector>
#include <iostream>
#include "ScalableElement.hpp"

class AppContext
{
public:
    AppContext();
    void addObject(ScalableElement* new_object);
    ScalableElement* getActiveObject();


private:
    std::vector<ScalableElement*> m_objects;
    size_t m_activeObjectIndex;
};

#endif // APP_CONTEXT_HPP