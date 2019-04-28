#pragma once

#include <string>

class StringUtils {
public:
	static void replace_last_new_line_char(std::string &str);
	static void cut_string(std::string &str, char c);
	static void cut_string_from_begining(std::string &str, size_t pos);
	static void strip_whitespaces(std::string &str);
};
