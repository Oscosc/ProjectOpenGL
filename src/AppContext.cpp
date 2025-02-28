#include "AppContext.hpp"


AppContext::AppContext() {}


Object* AppContext::getActiveObject()
{
    if(m_objects.empty()) return nullptr;
    return m_objects[m_activeObjectIndex];
}


void AppContext::addObject(Object* new_object)
{
    m_objects.push_back(new_object);
    return;
}
