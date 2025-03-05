#include "AppContext.hpp"


AppContext::AppContext(unsigned int screen_width, unsigned int screen_height) :
    SCR_WIDTH(screen_width),
    SCR_HEIGHT(screen_height),
    m_activeObjectIndex(-1),
    m_camera(Camera(glm::vec3(0.0f, 0.0f, 3.0f))),
    m_projection(glm::mat4(1.0f)),
    m_view(glm::mat4(1.0f)),
    m_cursorX(SCR_WIDTH / 2.0f),
    m_cursorY(SCR_HEIGHT / 2.0f)
{}


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


void AppContext::removeRays()
{
    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [](const std::unique_ptr<Object>& obj) {
        return dynamic_cast<Ray*>(obj.get()) != nullptr;
    }), m_objects.end());
}

glm::mat4 AppContext::getView(){return m_view;}

void AppContext::setView(glm::mat4 view) {m_view = view;}

glm::mat4 AppContext::getProjection() {return m_projection;}

void AppContext::setProjection(glm::mat4 projection) {m_projection = projection;}

Camera* AppContext::getCamera() {return &m_camera;}

float AppContext::getCursorX() {return m_cursorX;}

void AppContext::setCursorX(float value) {m_cursorX = value;}

float AppContext::getCursorY() {return m_cursorY;}

void AppContext::setCursorY(float value) {m_cursorY = value;}

bool AppContext::isMouseActive() {return m_mouseActive;}

void AppContext::switchMouseActive() {m_mouseActive = !m_mouseActive;}

bool AppContext::isFirstMouse() {return m_firstMouse;}

void AppContext::firstMouseDone() {m_firstMouse = false;}

float AppContext::getDeltaTime() {return m_deltaTime;}

void AppContext::setDeltaTime(float value) {m_deltaTime = value;}

float AppContext::getLastFrame() {return m_lastFrame;}

void AppContext::setLastFrame(float value) {m_lastFrame = value;}
