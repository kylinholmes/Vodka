#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <string_view>


class Config
{
   using sv = std::string_view;
public:
   int port;
   size_t logLevel;
   static Config* GetInstance(){
      static Config config;
      return &config;
   }
private:
   Config() :port(80), logLevel(){};
   
};
#endif 
