#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include <iostream>
#include <string>


class Locations
{
    private:
        std::string path;
        std::string root;
        std::string index;
    public:
        Locations();
        ~Locations();
        void setPath(const std::string& path);
        void setRoot(const std::string& root);
        void setIndex(const std::string& index);
        const std::string& getPath(void);
        const std::string& getRoot(void);
        const std::string& getIndex(void);
};

#endif