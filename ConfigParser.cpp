#include "ConfigParser.hpp"


ConfigParser::ConfigParser(std::string &str)
{
    std::string key, value;
    while(str.find('\n') != std::string::npos)
    {    
        // get key
        auto eq_pos = str.find('=');
        if (std::string::npos == eq_pos) return;
        key = str.substr(0, eq_pos);
        str.erase(0, eq_pos + 1);

        // get value
        auto n_pos = str.find('\n');
        if (std::string::npos == n_pos) return;
        value = str.substr(0, n_pos);
        str.erase(0, n_pos + 1);

        this->config_dict.emplace(key, value);
    }
}

std::string ConfigParser::get_value(const std::string &str)
{
    return this->config_dict[str];
}

int ConfigParser::get_value_as_int(const std::string &str)
{
    int val;
    val = std::atoi(config_dict[str].c_str());
    return val;
}

bool ConfigParser::get_value_as_bool(const std::string &str)
{
    bool val;
    if (config_dict[str] == "true") val = true;
    else if (config_dict[str] == "false") val = false;

    return val;
}

float ConfigParser::get_value_as_float(const std::string &str)
{
    float val;
    val = (float)std::atof(config_dict[str].c_str());
    return val;
}
