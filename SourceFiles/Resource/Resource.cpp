#include "Resource\Resource.h"

void Resource::SetResourceId(unsigned int value)
{
	_resourceId = value;
}

void Resource::SetResourceName(std::string name)
{
	_resourceName = name;
}

const unsigned int&	Resource::GetResourceId()
{
	return _resourceId;
}

const std::string& Resource::GetResourceName()
{
	return _resourceName;
}