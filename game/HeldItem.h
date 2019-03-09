#pragma once
#include <string>
class HeldItem
{
public:
	std::string Name;
	std::string Id;
	int Count = 1;
	HeldItem(std::string name, std::string id);
	~HeldItem();

	bool operator==(const HeldItem& rhs)
	{
		return this->Name == rhs.Name && this->Id == rhs.Id;
	}

	bool operator!=(const HeldItem& rhs)
	{
		return this->Name != rhs.Name && this->Id != rhs.Id;
	}
};