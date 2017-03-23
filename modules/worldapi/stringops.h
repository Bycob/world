
#ifndef STRING_OPS
#define STRING_OPS

#include <vector>
#include <sstream>

inline std::vector<std::string> split(const std::string & splitted, const char delim, bool trimEmpty = false) {
	std::vector<std::string> result;
	std::stringstream ss;
	ss.str(splitted);
	std::string line;

	while (getline(ss, line, delim)) {
		if (trimEmpty && line.size() == 0) continue;
		result.push_back(line);
	}
	return result;
}

inline std::string trimSpaces(const std::string & item) {
	int startIndex = 0, endIndex = item.size();
	for (int i = 0; i < item.size(); i++) {
		if (item[i] == ' ' || item[i] == '\t') {
			startIndex++;
		}
		else {
			break;
		}
	}
	for (int i = endIndex - 1; i >= startIndex; i++) {
		if (item[i] == ' ' || item[i] == '\t') {
			endIndex--;
		}
		else {
			break;
		}
	}
	return item.substr(startIndex, endIndex - startIndex);
}

inline bool startsWith(const std::string & item, const std::string & prefix) {
	if (item.size() < prefix.size()) {
		return false;
	}

	//C'est pas drôle
	for (int c = 0; c < prefix.size(); c++) {
		//vous avez ri ?
		if (item[c] != prefix[c]) {
			return false;
		}
	}

	return true;
}

inline bool endsWith(const std::string & item, const std::string & suffix) {
	if (item.size() < suffix.size()) {
		return false;
	}

	for (int i = item.size(), j = suffix.size(); j >= 0; i--, j--) {
		if (item[i] != suffix[j]) {
			return false;
		}
	}

	return true;
}

#endif //STRING_OPS