#pragma once

/**
 * @brief Generic template class defined to construct Singleton.
 */
template<typename T>
class Singleton
{
public:
    
    /**
     * @brief Return the instance of the singleton (create it if doesn't exist).
     */
    static T& getInstance() {
        static T instance;
        return instance;
    }

    /**
     * @brief Removed copy constructor.
     */
    Singleton(const Singleton&) = delete;

    /**
     * @brief Removed move constructor.
     */
    Singleton& operator=(const Singleton&) = delete;

protected:

    /**
     * @brief Constructor for inherited class. 
     */
    Singleton() = default;

    /**
     * @brief Destructor for inherited class.
     */
    ~Singleton() = default;
};