#include <fstream>
#include "PPM.h"

PPM::PPM(const int &width, const int &height, const int &maxValue) : PortableAnymap(width, height, maxValue) {
}

PPM PPM::load(const std::string &path) {
    std::ifstream file(path);
    std::string magicNumber;
    file >> magicNumber;
    int width, height, maxValue;
    file >> width;
    file >> height;
    file >> maxValue;
    PPM image(width, height, maxValue);

    int i = 0;
    float r, g, b;
    while (file >> r >> g >> b) {
        image(i / width, i % width).color = Vector3<float>(r, g, b);
        ++i;
    }
    file.close();
    return image;
}

std::ostream &operator<<(std::ostream &os, const PPM &ppm) {
    os << "P3" << std::endl;
    os << ppm.width << " " << ppm.height << std::endl;
    os << ppm.maxValue << std::endl;
    for (int row = 0; row < ppm.height; ++row) {
        const Vector3<float> &firstPixel = ppm(row, 0).color;
        os << firstPixel.x << " " << firstPixel.y << " " << firstPixel.z;
        for (int col = 1; col < ppm.width; ++col) {
            const Vector3<float> &pixel = ppm(row, col).color;
            os << " " << pixel.x << " " << pixel.y << " " << pixel.z;
        }
        os << std::endl;
    }
    return os;
}

void PPM::save(const std::string &path) const {
    std::ofstream file(path);
    file << *this;
    file.close();
}
