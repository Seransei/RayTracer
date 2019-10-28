#ifndef IMAGE_SYNTHESIS_PORTABLEANYMAP_H
#define IMAGE_SYNTHESIS_PORTABLEANYMAP_H

#include <string>
#include <ostream>
#include "Vector3.hpp"

template<typename T>
class PortableAnymap {
protected:
    struct Pixel{
        Vector3<float> color;
    };
    
    PortableAnymap(const int &width, const int &height, const int &maxValue) {
        this->width = width;
        this->height = height;
        this->maxValue = maxValue;
        pixelMatrix = new Pixel[height * width];
    }

public:
    int width, height, maxValue;
    Pixel *pixelMatrix;

    ~PortableAnymap() {        
        delete[] pixelMatrix;
    }

    virtual void save(const std::string &path) const = 0;

    Pixel& operator()(const int &x, const int &y){
        return pixelMatrix[y * height + x];
    }
    const Pixel& operator()(const int &x, const int &y) const{
        return pixelMatrix[y * height + x];
    }
};

#endif //IMAGE_SYNTHESIS_PORTABLEANYMAP_H
