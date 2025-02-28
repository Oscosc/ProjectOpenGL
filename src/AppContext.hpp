#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <vector>
#include "Object.hpp"

class AppContext
{
public:
    AppContext();
    void addObject(Object* new_object);
    Object* getActiveObject();


private:
    std::vector<Object*> m_objects;
    size_t m_activeObjectIndex;
};

#endif // APP_CONTEXT_HPP