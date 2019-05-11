#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

/*
 * This library allows only for simply JSON schemas
 *    only simple array with objects in theme, without any
 *    additional nesting
 */
namespace Json
{
	enum class Type
	{
		Invalid,
		Array,
		Null,
		Bool,
		String,
		Int,
		Object,
	};

    class Json
    {
    private:
        Type type = Type::Invalid;

        union 
        {
            bool bool_value;
            int integer_value;
            std::string *string_value;
            std::map<std::string, Json> *object;
            std::vector<Json> *array_value;
        };
        
    public:
        Json();
        Json(const std::vector<Json> &vector);
        Json(const Json&);
        Json(nullptr_t p);
        Json(bool value);
        Json(const std::string &str);
        Json(int val);
        Json(const Type &type);
        Json(const std::map<std::string, Json> &map);
        Json(const std::string &str, const Json &value);
        ~Json();

        std::string to_string() const;

        const Json& operator[](const std::string &key) const;
        const Json& operator[](const char *key) const;
        const Json& operator[](const int index) const;
        Json& operator=(const Json& other);
        operator int() const;
        operator std::string() const;

        void add_pair(const std::string &str, const Json &value);
        void append(const Json &json);

        size_t get_size() const;
        std::string get_type() const;
        std::string get_value_as_string() const;
        std::vector<Json> get_value_as_vector() const;
        int get_value_as_int() const;
        std::vector<std::string> get_keys() const;
        bool check_if_key_exists(const std::string &key) const;

        static Json parse_from_string(const std::string &encodig);

        friend std::ostream& operator<<(std::ostream &out, const Json &json);
    };

    
}
