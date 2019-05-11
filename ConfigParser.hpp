#pragma once

#include <string>
#include <map>

class ConfigParser
{
private:
    std::map<std::string, std::string> config_dict;
public:
    ConfigParser(std::string &str);

    std::string get_value(const std::string &str);
    int get_value_as_int(const std::string &str);
    bool get_value_as_bool(const std::string &str);
    float get_value_as_float(const std::string &str);
};