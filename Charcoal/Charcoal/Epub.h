#pragma once
#include "Library.h"
class Epub : public Library
{
	public:
		book add(PWSTR path);
		void grayscaleEpub(PWSTR Path);
	private:
		void grayscaleImage(unsigned char* imageData, int width, int height);
		void contrastImage(unsigned char* imageData, int width, int height, int value);
};

