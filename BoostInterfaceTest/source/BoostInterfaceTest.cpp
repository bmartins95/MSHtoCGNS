#define BOOST_TEST_MODULE BoostInterfaceTestModule

#include <BoostInterface/Test.hpp>
#include <BoostInterface/Filesystem.hpp>

TestSuite(Filesystem)
	TestCase(CreateDirectory) {
		std::string Path = "Test/";
		createDirectory(Path);
		
		check(boost::filesystem::exists(Path) == true);
	}
	
	TestCase(DeleteDirectory) {
		std::string Path = "Test/";
		deleteDirectory(Path);
		
		check(boost::filesystem::exists(Path) == false);
	}
TestSuiteEnd()