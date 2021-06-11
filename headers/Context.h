#pragma once
#include <map>
#include "Value.h"

namespace az::cli
{

class Context
{
	// TODO: implement smart Key structure for the map to store ids and names
	std::map<int,Value> map;
public:
	Value get(int id) const {
		auto it = map.find(id);
		return (it != map.end()) ?
			it->second : Value();
	}
	template<typename T> bool get(int id, T& value) const {
		if (has(id)) {
			value = T(get(id));
			return true;
		}
		return false;
	}
	const std::map<int,Value>& get() const {
		return map;
	}
	Value& operator[](int id) {
		return map[id];
	}
	Value operator[](int id) const {
		return get(id);
	}
	Value at(int id) const {
		return get(id);
	}
	bool has(int id) const {
		return map.count(id) > 0;
	}
	bool empty() const {
		return map.empty();
	}
};

}
