#include "Json.hpp"

#include <set>
#include <stack>
#include <iostream>
#include <utility>

namespace
{
    Json::Json __json(nullptr);
}

namespace Json
{
    static Json _parse_from_string(const std::string &encoding, size_t &pos);

    Json::Json()
    {
        this->type = Type::Invalid;
    }

    Json::Json(const std::vector<Json> &vector)
    {
        this->type = Type::Array;
        this->array_value = new std::vector<Json>(vector);
    }

    Json::Json(const Json& other)
    {
        switch(other.type)
        {
            case Type::Invalid:
                this->type = other.type;
                break;
            case Type::Int:
                this->type = other.type;
                this->integer_value = other.integer_value;
                break;
            case Type::Object:
                this->type = other.type;
                this->object = new std::map<std::string, Json>(*other.object);
                break;
            case Type::String:
                this->type = other.type;
                this->string_value = new std::string(*other.string_value);
                break;
            case Type::Bool:
                this->type = other.type;
                this->bool_value = other.bool_value;
                break;
            case Type::Null:
                this->type = other.type;
                break;      
            case Type::Array:
                this->type = other.type;
                this->array_value = new std::vector<Json>(*other.array_value);
                break;       
        }
    }

    Json::Json(nullptr_t p)
    {
        this->type = Type::Null;
    }

    Json::Json(bool value)
    {
        this->type = Type::Bool;
        this->bool_value = value;
    }

    Json::Json(const std::string &str)
    {
        this->type = Type::String;
        this->string_value = new std::string(str);       
    }
    
    Json::Json(int val)
    {
        this->type = Type::Int;
        this->integer_value = val;  
    }

    Json::Json(const std::string &str, const Json &value)
    {
        this->type = Type::Object;
        this->object = new std::map<std::string, Json>;

        this->object->emplace(std::make_pair(str, value));
    }

    Json::Json(const Type &type)
    {
        switch(type)
        {
            case Type::Array:
                this->type = Type::Array;
                break;
        }
    }

    Json::Json(const std::map<std::string, Json> &map)
    {
        this->type = Type::Object;
        this->object = new std::map<std::string, Json>(map);
    }

    void Json::add_pair(const std::string &str, const Json &value)
    {
        if (this->type != Type::Object) return;
        this->object->emplace(std::make_pair(str, value));
    }

    std::string Json::get_type() const
    {
        switch(this->type)
        {
            case Type::Object: return "OBJECT";
            case Type::Array: return "ARRAY";
            case Type::String: return "STRING";
            case Type::Int: return "INT";
            case Type::Invalid: return "INVALID";
            case Type::Bool: return "BOOL";
            case Type::Null: return "NULL";
        }
    }

    std::string Json::to_string() const
    {
        switch(this->type)
        {
            case Type::Bool: return this->bool_value ? "true" : "false";
            case Type::Null: return "null";
            case Type::String: return "\"" + *string_value + "\"";
            case Type::Int: return std::to_string(integer_value);
            case Type::Object:
            {
                std::string ret = "{";
                for (const auto &ob : *this->object)
                {
                    ret += "\"" + ob.first + "\"";
                    ret += ':';
                    ret += ob.second.to_string() + ",";   
                }
                return ret.replace(ret.length() - 1, 1, "}");
            }
            case Type::Array:
            {
                std::string ret = "[";
                for (const auto &ob : *this->array_value)
                {
                    ret += ob.to_string();
                    ret += ",";
                }
                return ret.replace(ret.length() - 1, 1, "]");
            }
        }
    }

    Json& Json::operator=(const Json& other)
    {
        if (this == &other) return *this;

        switch(other.type)
        {
            case Type::Invalid:
                this->type = other.type;
                break;
            case Type::Int:
                this->type = other.type;
                this->integer_value = other.integer_value;
                break;
            case Type::Object:
                this->type = other.type;
                this->object = new std::map<std::string, Json>(*other.object);
                break;
            case Type::String:
                this->type = other.type;
                this->string_value = new std::string(*other.string_value);
                break;
            case Type::Bool:
                this->type = other.type;
                this->bool_value = other.bool_value;
                break;
            case Type::Null:
                this->type = other.type;
                break;      
            case Type::Array:
                this->type = other.type;
                this->array_value = other.array_value;
                break;       
        }
        return *this;
    }

    Json::operator int() const
    {
        if (this->type == Type::Int)
        {
            return this->integer_value;
        }
        return 0;
    }

    void Json::append(const Json &json)
    {
        if (this->type != Type::Array) return; 
        this->type = Type::Array;
        this->array_value->emplace_back(json);

    }

    Json::operator std::string() const
    {
        if (this->type == Type::String)
        {
            return *this->string_value;
        }
        return std::string("");
    }

    Json Json::parse_from_string(const std::string &encoding)
    {
        size_t pos = 0;
        return _parse_from_string(encoding, pos);
    }

    static Json _parse_from_string(const std::string &encoding, size_t &pos)
    {
        const char *WHITE_SPACE = " \r\n\t";
        const std::set<char> NUMBERS = {'0', '1', '2', '3', '4',
                                         '5', '6', '7', '8', '9'};
        const std::string NUMBERS_STRING = "0123456789";

        //std::cout << "STRING TO PARSE: " << encoding.substr(pos) << std::endl;

        while(pos < encoding.size())
        {
            pos = encoding.find_first_not_of(WHITE_SPACE, pos);
            switch(encoding[pos])
            {
                case '\"':
                {
                    auto quot_pos = encoding.find("\"", pos + 1);
                    auto last = pos;
                    pos = quot_pos + 1;
                    return Json(encoding.substr(last + 1, quot_pos - (last + 1)));
                } break;
                case '{':
                {
                    std::map<std::string, Json> objects;
                    objects.clear();
                    pos++;
                    while(encoding[pos] != '}' and pos < encoding.size())
                    {
                        pos = encoding.find_first_not_of(WHITE_SPACE, pos);
                        if (encoding[pos] == '}') break;
                        
                        // parse name
                        pos = encoding.find('\"', pos) + 1;
                        auto quot_pos = encoding.find('\"', pos);
                        std::string key(encoding.substr(pos, quot_pos - pos));
                        pos = encoding.find(':', pos);
                        
                        // parse value
                        Json value = _parse_from_string(encoding, pos);
                        objects.emplace(std::make_pair(key, value));
                    }
					pos = encoding.find('}', pos);
					pos++;
                    return Json(objects);
                } break;
                case '[':
                {
                    std::vector<Json> ret;
                    ret.clear();
                    pos++;
                    while(encoding[pos] != ']' and pos < encoding.size())
                    {
                        pos = encoding.find_first_not_of(WHITE_SPACE, pos);
						if(encoding[pos] == ']') break;
                        ret.push_back(_parse_from_string(encoding, pos));
                    }
					pos = encoding.find(']', pos);
					pos++;
                    return Json(ret);
                } break;
                default:
                {
                    auto number_pos = NUMBERS.find(encoding[pos]);
                    if (number_pos == NUMBERS.end()) break;
                    else 
                    {
                        auto last_pos = encoding.find_first_not_of(NUMBERS_STRING, pos);
                        auto string_to_be_int = encoding.substr(pos, last_pos - pos);
                        int value = std::stoi(string_to_be_int);
                        pos = last_pos;
                        return Json(value);
                    }
                } break;
            }
            pos++;
        }
        return Json(nullptr);
    }

     Json::~Json()
     {
         switch(this->type)
         {
             case Type::String:
                delete string_value;
                return;
            case Type::Object:
                delete this->object;
                return;
            case Type::Array:
                delete this->array_value;
                return;
         }
    }

    const Json& Json::operator[](const int index) const
    {
        if (this->type != Type::Array) return __json;
        if (this->array_value->size() <= index) return __json;
        
        return (*this->array_value)[index];
    }

    const Json& Json::operator[](const std::string &key) const
    {
        if (this->type != Type::Object)
        {
            return __json;
        }

        const auto result = object->find(key);
        if (result == object->end())
        {
            return __json;
        }

        return result->second;
    }
    const Json& Json::operator[](const char *k) const
    {
        std::string key(k);

        return (*this)[key];
    }

    size_t Json::get_size() const
    {
        switch(this->type)
        {
            case Type::Array: return (*this->array_value).size();
            case Type::Object: return (*this->object).size();
            default: return 0;
        }
    }

    std::vector<Json> Json::get_value_as_vector() const
    {
        switch(this->type)
        {
            case Type::Array: return (*this->array_value);
            default: return {};
        }
    }

    std::string Json::get_value_as_string() const
    {
        switch(this->type)
        {
            case Type::String: return (*this->string_value);
            case Type::Int: return std::to_string(this->integer_value);
            default: return ""; 
        }
    }

    int Json::get_value_as_int() const
    {
        if (this->type == Type::Int) return this->integer_value;
        return -1;
    }

    std::vector<std::string> Json::get_keys() const
    {
        std::vector<std::string> ret;
        if (this->type != Type::Object) return ret;

        for (const auto &v : *this->object)
        {
            ret.push_back(v.first);
        }

        return ret;
    }

	bool Json::check_if_key_exists(const std::string &key) const
	{
		if(this->type != Type::Object)
		{
			return false;
		}

		return this->object->end() == this->object->find(key) ? false : true;
	}

    std::ostream& operator<<(std::ostream &out, const Json &json)
    {
        switch(json.type)
        {
            case Type::Invalid:
                out << "Invalid, no value";
                break;
            case Type::Int:
                out << json.integer_value;
                break;
            case Type::Object:
                out << "Object with: " << (*json.object).size() << " length."; 
                break;
            case Type::String:
                out << (*json.string_value);
                break;
            case Type::Bool:
                out << "Boolean";
                break;
            case Type::Null:
                out << "Null";
                break;      
            case Type::Array:
                out << "Array with: " << (*json.array_value).size() << " elemnts";
                break;       
        }
        return out;
    }
}

