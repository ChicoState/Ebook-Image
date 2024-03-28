#pragma once
#include "Library.h"
class Epub : public Library
{
	public:
		book add(PWSTR path);
};

