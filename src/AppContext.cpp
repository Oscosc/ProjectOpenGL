#include "AppContext.hpp"


AppContext::AppContext() : m_activeObjectIndex(-1) {}


ScalableElement* AppContext::getActiveObject()
{
    if(m_objects.empty()) return nullptr;
    return dynamic_cast<ScalableElement*>(m_objects[m_activeObjectIndex]);
}


void AppContext::addObject(Object* new_object)
{
    m_objects.push_back(new_object);
    if(m_activeObjectIndex == -1) m_activeObjectIndex++;
    return;
}


std::vector<Object*>::iterator AppContext::begin()
{
    return m_objects.begin();
}


std::vector<Object*>::const_iterator AppContext::begin() const
{
    return m_objects.cbegin();
}


std::vector<Object*>::iterator AppContext::end()
{
    return m_objects.end();
}


std::vector<Object*>::const_iterator AppContext::end() const
{
    return m_objects.cend();
}