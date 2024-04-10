#pragma once
#include "Library.h"
class Epub : public Library
{
	public:
		book add(PWSTR path);
		void grayscaleEpub(std::string Path);
	private:
		void grayscaleImage(unsigned char* imageData, int width, int height);
};

