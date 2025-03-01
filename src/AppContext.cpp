#include "AppContext.hpp"


AppContext::AppContext() : m_activeObjectIndex(-1) {}


ScalableElement* AppContext::getActiveObject()
{
    if(m_objects.empty()) return nullptr;
    return m_objects[m_activeObjectIndex];
}


void AppContext::addObject(ScalableElement* new_object)
{
    m_objects.push_back(new_object);
    if(m_activeObjectIndex == -1) m_activeObjectIndex++;
    return;
}
