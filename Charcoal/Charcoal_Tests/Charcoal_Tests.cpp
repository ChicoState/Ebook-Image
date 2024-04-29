#include "pch.h"
#include "CppUnitTest.h"
#include "Charcoal.h"
#include "Library.h"
#include "Epub.h"
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
		TEST_METHOD(WstrConversion)
		{
			Library l;
			PWSTR test = L"test";
			std::string con = l.wstrtostr(test);
			Assert::AreEqual(con, std::string("test"));
		}
		TEST_METHOD(Epub_Add)
		{
			Library l;
			PWSTR file = L"..\\..\\Charcoal\\libepub-master\\books\\PrideAndPrejudice.epub";
			std::string out = l.add(file);
			Assert::AreEqual(out, std::string("Pride and Prejudice"));

		}
		TEST_METHOD(PDF_ADD)
		{

		}
		TEST_METHOD(AZW3_ADD)
		{

		}
		TEST_METHOD(printall)
		{
			Library l;
			PWSTR file = L"..\\..\\Charcoal\\libepub-master\\books\\PrideAndPrejudice.epub";
			l.add(file);
			std::string out = l.printall();
			Assert::AreEqual(out, std::string("<li><span>' + \"Pride and Prejudice\"+ '</span></li>"));
		}
		TEST_METHOD(getstringdata)
		{
			Library l;
			PWSTR file = L"..\\..\\Charcoal\\libepub-master\\books\\PrideAndPrejudice.epub";
			l.add(file);
			std::string out = l.getStringData("Pride and Prejudice");
			Assert::AreEqual(out, std::string("Title: Pride and Prejudice\nAuthor: Jane Austen\nPublisher: \nContributor: \nRights: Public domain in the USA.\nFormat: \nDate: 2014-07-04T14:27:21.418689+00:00\nLanguage: en\nDescription: \n"));
		}
	};
}
