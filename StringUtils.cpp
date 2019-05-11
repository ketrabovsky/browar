#include "StringUtils.hpp"


void StringUtils::replace_last_new_line_char(std::string &str) {
    auto last_pos = str.size() - 1;

    if('\n' == str[last_pos]) {
        str.replace(last_pos, 1, "\0");
    }
}

void StringUtils::cut_string(std::string &str, char c) {
	auto c_pos = str.find(c);

	if (c_pos == std::string::npos) {
		return;
	}

	str.erase(c_pos, str.size() - c_pos);
}

void StringUtils::cut_string_from_begining(std::string &str, size_t pos)
{
	if (pos == std::string::npos) {
		return;
	}

	str.erase(0, pos);
}

void StringUtils::strip_whitespaces(std::string &str)
{
	for (size_t index = 0; index < str.size(); index++)
	{
		if (str[index] == '\n')
		{
			str.erase(index, 1);
			index--;
			continue;
		} else if (str[index] == '\t')
		{
			str.erase(index, 1);
			index--;
			continue;
		} else if (str[index] == ' ')
		{
			str.erase(index, 1);
			index--;
			continue;
		}
	}
}
