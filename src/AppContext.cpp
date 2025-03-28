#include "AppContext.hpp"


AppContext::AppContext(unsigned int screen_width, unsigned int screen_height, glm::vec3 backgroundColor, glm::vec3 lightColor) :
    SCR_WIDTH(screen_width),
    SCR_HEIGHT(screen_height),
    m_backgroundColor(backgroundColor),
    m_lightColor(lightColor),
    m_displayMode(STANDARD_DISPLAY_MODE),
    m_activeObjectIndex(-1),
    m_camera(Camera(glm::vec3(0.0f, 0.0f, 3.0f))),
    m_projection(glm::mat4(1.0f)),
    m_view(glm::mat4(1.0f)),
    m_cursor({SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f})
{}


ScalableElement* AppContext::getActiveAsScalable()
{
    if(m_objects.empty()) return nullptr;
    return dynamic_cast<ScalableElement*>(m_objects[m_activeObjectIndex].get());
}


Object *AppContext::getActiveAsObject()
{
    return m_objects.empty() ? nullptr : m_objects[m_activeObjectIndex].get();
}


void AppContext::addObject(AppContext::uniqueObject new_object)
{
    m_objects.push_back(std::move(new_object));
    if(m_activeObjectIndex == -1) m_activeObjectIndex++;
    return;
}


AppContext::uniqueObjectsList::iterator AppContext::begin() {return m_objects.begin();}
AppContext::uniqueObjectsList::const_iterator AppContext::begin() const {return m_objects.cbegin();}

AppContext::uniqueObjectsList::iterator AppContext::end() {return m_objects.end();}
AppContext::uniqueObjectsList::const_iterator AppContext::end() const {return m_objects.cend();}


void AppContext::nextObject()
{
    m_activeObjectIndex = (m_activeObjectIndex + 1) % size();
}


void AppContext::previousObject()
{
    // Modulo ne fonctionne pas pour -1 donc méthode if/else
    if(m_activeObjectIndex > 0) m_activeObjectIndex--;
    else m_activeObjectIndex = size() - 1;
}


unsigned int AppContext::size() const {return m_objects.size();}


void AppContext::clearRays()
{
    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [](const std::unique_ptr<Object>& obj) {
        return dynamic_cast<Ray*>(obj.get()) != nullptr;
    }), m_objects.end());
}


Object* AppContext::getObject(size_t index) {
    if (index >= m_objects.size()) throw std::out_of_range("Index out of range");
    return m_objects[index].get();
}

glm::vec3 AppContext::getBackgroundColor() {return m_backgroundColor;}

glm::vec3 AppContext::getLightColor() {return m_lightColor;}

unsigned int AppContext::getDisplayMode() const {return m_displayMode;}
void AppContext::setDisplayMode(unsigned int value) {m_displayMode = value;}

glm::mat4 AppContext::getView(){return m_view;}
void AppContext::setView(glm::mat4 view) {m_view = view;}

glm::mat4 AppContext::getProjection() {return m_projection;}
void AppContext::setProjection(glm::mat4 projection) {m_projection = projection;}

Camera* AppContext::getCamera() {return &m_camera;}

glm::vec2 AppContext::getCursor() {return m_cursor;}
void AppContext::setCursor(glm::vec2 value) {m_cursor = value;}
void AppContext::setCursor(float x, float y) {m_cursor = {x, y};}

bool AppContext::isMouseActive() {return m_mouseActive;}
void AppContext::switchMouseActive() {m_mouseActive = !m_mouseActive;}

bool AppContext::isFirstMouse() {return m_firstMouse;}
void AppContext::firstMouseDone() {m_firstMouse = false;}

float AppContext::getDeltaTime() {return m_deltaTime;}
void AppContext::setDeltaTime(float value) {m_deltaTime = value;}

float AppContext::getLastFrame() {return m_lastFrame;}
void AppContext::setLastFrame(float value) {m_lastFrame = value;}

unsigned int AppContext::getActiveIndex() {return m_activeObjectIndex;}
