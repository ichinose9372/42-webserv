#ifndef EXCLUSIVEPATH_HPP
#define EXCLUSIVEPATH_HPP


#include <string>

class ExclusivePath 
{

    public:
        enum PathType {NONE, ROOT, ALIAS};
        void setRoot(const std::string& path);
        void setAlias(const std::string& path);
        const std::string& getPath(void);
        void setPathType(PathType pathType);
    private:
        std::string path;
        PathType pathType;
};

#endif