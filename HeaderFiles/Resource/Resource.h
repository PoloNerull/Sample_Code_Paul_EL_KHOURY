#pragma once

#include <string>

class Resource
{
public:
	Resource() = default;
	~Resource() = default;

	void					SetResourceId(unsigned int value);
	void					SetResourceName(std::string name);

	const unsigned int&		GetResourceId();
	const std::string&		GetResourceName();
private:

	unsigned int			_resourceId;
	std::string				_resourceName;
};