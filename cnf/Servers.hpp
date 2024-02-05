#ifndef SERVERS_HPP
#define SERVERS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Locations.hpp"
#include <sstream>
#include <unistd.h>
#include <set>
#include <algorithm>


class Locations;

class Servers
{
    private:
        size_t port;
        std::vector<std::string> indexs;
        std::string sever_name;
        bool serverNameset;
        bool portset;
        std::vector<Locations> locations;
        std::map<int, std::string> error_pages;
        size_t client_max_body_size;
        std::string root;
    public:
        Servers();
        ~Servers();
        //setter
        void setPort(const size_t& port);
        void setIndex(const std::string& index);
        void setSeverNames(const std::string& sever_name);
        void setLocations(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
        void setClientMaxBodySize(const std::string& client_max_body_size);
        void setRoot(const std::string& root);
        void setErrorPage(const std::string statuscode, const std::string error_page);
        //getter
        size_t getPort(void) const;
        const std::string& getHost(void) const;
        const std::string& getServerNames(void) const;
        const std::vector<std::string>& getIndexs(void) const;
        size_t getClientMaxBodySize(void) const;
        const std::vector<Locations>& getLocations(void) const; 
        const std::string& getRoot(void) const;
        const std::map<int, std::string > getErrorpage(void) const;
        //check functions
        void  isPathDuplicate(const std::string& path);
        void  processSingleValueDirective(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, Locations& location, const std::string& directive);
        void  processMultiValueDirective(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, Locations& location, const std::string& directive);

       
};

void  removeTrailingSemicolon(std::string& str);

#endif