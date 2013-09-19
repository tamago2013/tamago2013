#ifndef TKG_CONFIG_HPP
#define TKG_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cctype>
#include "tkg-utility.hpp"

#include <iostream>

namespace tkg
{

/*
class Config : public std::map< std::string, std::map<std::string, std::string> >
{
        int parseKey(const std::string &str, int p)
        {
            for(uint i=p; i<str.size(); i++)
            {
                if( !isalnum(str[i]) && str[i]!='-' ) return i;
            }
        }

        int parseLine(const std::string &str, int s)
        {
            int t = parseKey(str, s);
            printf("%s\n", str.substr(s, t-s).c_str());

            if(str[++t] != '{') throw t;

            s = t;
            t = parseKey(str, t);
            printf("%s\n", str.substr(s, t-s).c_str());

            if(str[++t] != '}') throw t;
        }

        int parseBlock(const std::string &str, int s)
        {

                s = parseLine(str, s);
        }

        int parseGroup(const std::string &str, int s)
        {
            int t = parseKey(str, s);
            printf("%s\n", str.substr(s, t-s).c_str());

            if(str[++t] != '{') throw t;

            s = t;
            t = parseBlock(str, t);
            printf("%s\n", str.substr(s, t-s).c_str());

            if(str[++t] != '}') throw t;
        }

        int parseFile(const std::string &str, int s)
        {
            while(s != str.size())
            {
                s = parseGroup(str, s);
            }
        }

    public:

        bool load(const std::string &file)
        {
            std::ifstream fin(file.c_str());
            if(!fin) return false;

            std::string str,buf;
            while(fin >> buf)
            {
                str += buf;
            }

            try { printf("%d %d\n", parseFile(str, 0), str.size()); }
            catch(int pos) { printf("error: %d\n", pos); }
            return 0;
        }
};
*/

class ConfigValue : public std::string
{
    public:

        bool parse(const std::string &str)
        {
            for(size_type p=0,n=0; true; p=n+1)
            {
                n = str.find_first_of(";", p);
                if(n == std::string::npos) break;

                assign( str.substr(p,n-p) );
            }
            return true;
        }

        void print(std::string key)
        {
            std::cout << key << " = " << (*this) << std::endl;
        }
};

class ConfigGroup : public std::map<std::string, ConfigValue>
{
    public:

        bool parse(const std::string &str)
        {
            for(size_type p=0,s=0,g=0; true; p=g+1)
            {
                s = str.find_first_of(':', p);
                g = str.find_first_of(';', p);

                if(s == std::string::npos) break;
                if(g == std::string::npos) break;

                ConfigValue val;
                if( !val.parse(str.substr(s+1, g-s)) ) return false;

                insert( make_pair(str.substr(p,s-p), val) );
            }
            return true;
        }

        void print(std::string key)
        {
            for(std::map<std::string, ConfigValue>::iterator it=begin(); it!=end(); it++)
            {
                it->second.print(key + std::string("[") + it->first + std::string("]"));
            }
        }
};

class ConfigFile : public std::map<std::string, ConfigGroup>
{
    public:

        bool load(const std::string &file)
        {
            std::ifstream fin(file.c_str());
            if(!fin) return false;

            std::string str,buf;
            while(fin >> buf)
            {
                str += buf;
            }

            str = tkg::removeComment(str);

            return parse(str);
        }

        bool parse(const std::string &str)
        {
            for(size_type p=0,s=0,g=0; true; p=g+1)
            {
                s = str.find_first_of('{', p);
                g = str.find_first_of('}', p);

                if(s == std::string::npos) break;
                if(g == std::string::npos) break;

                ConfigGroup val;
                if( !val.parse(str.substr(s+1, g-s-1)) ) return false;

                insert( make_pair(str.substr(p,s-p), val) );
            }
            return true;
        }

        void print()
        {
            for(std::map<std::string, ConfigGroup>::iterator it=begin(); it!=end(); it++)
            {
                it->second.print(std::string("[") + it->first + std::string("]"));
            }
        }
};

}

#endif // TKG_CONFIG_HPP

