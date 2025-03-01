#ifndef SCALABLE_ELEMENT_HPP
#define SCALABLE_ELEMENT_HPP

class ScalableElement {
public:
    virtual void next() = 0;
    virtual void previous() = 0;
    virtual void switchMode() {altModeOn = !altModeOn;}
    
    bool isAltMode() {return altModeOn;}

protected:
    bool altModeOn = false;
};

#endif //SCALABLE_ELEMENT_HPP