#include "AppContext.hpp"


AppContext::AppContext() : m_activeObjectIndex(-1) {}


ScalableElement* AppContext::getActiveObject()
{
    if(m_objects.empty()) return nullptr;
    return dynamic_cast<ScalableElement*>(m_objects[m_activeObjectIndex].get());
}


void AppContext::addObject(AppContext::uniqueObject new_object)
{
    m_objects.push_back(std::move(new_object));
    if(m_activeObjectIndex == -1) m_activeObjectIndex++;
    return;
}


AppContext::uniqueObjectsList::iterator AppContext::begin()
{
    return m_objects.begin();
}


AppContext::uniqueObjectsList::const_iterator AppContext::begin() const
{
    return m_objects.cbegin();
}


AppContext::uniqueObjectsList::iterator AppContext::end()
{
    return m_objects.end();
}


AppContext::uniqueObjectsList::const_iterator AppContext::end() const
{
    return m_objects.cend();
}


unsigned int AppContext::size() const
{
    return m_objects.size();
}