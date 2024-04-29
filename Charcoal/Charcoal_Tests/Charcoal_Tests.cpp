#include "pch.h"
#include "CppUnitTest.h"
#include "Charcoal.h"
#include "Library.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CharcoalTests
{
	TEST_CLASS(CharcoalTests)
	{
	public:
		
		TEST_METHOD(FileExtension)
		{
			Library l;
			std::string file = "test.jpg";
			std::string ext = l.getFileExtension(file);
			Assert::AreEqual(ext, std::string("jpg"));
		}
		TEST_METHOD(AddBook)
		{
			Library l;
			PWSTR test = L"test";
			std::string con = l.wstrtostr(test);
			Assert::AreEqual(con, std::string("test"));
		}
	};
}
