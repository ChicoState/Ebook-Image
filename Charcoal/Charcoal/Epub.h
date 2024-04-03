#pragma once
#include "Library.h"
class Epub : public Library
{
	public:
		book add(PWSTR path);
	private:
		void grayscaleBook(PWSTR Path);
		void grayscaleImage(unsigned char* imageData, int width, int height);
};

