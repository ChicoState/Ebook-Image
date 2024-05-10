#pragma once
#include "Library.h"
class Epub : public Library
{
	public:
		book add(std::string path);
		void grayscaleEpub(std::string path);
	private:
};

