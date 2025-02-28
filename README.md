# xml2cpp
	Generate corresponding cpp reading and writing code according to xml file

## Usage
```
Usage: python generate.py [input.xml output.hpp]
Usage: python generate_xsd.py [input.xml output.xsd]
```
	Local test environment: win10 vs2022 python3.13.2

## Input and output
	input.xml is the xml file to be parsed,
	output.hpp is the generated cpp file.
	output.xsd is the generated xsd file.

	In the xml definition file(input.xml), 
	1. nodeCount is a special attribute that identifies the maximum number of times the current node appears. 
	   The default value is 2, which means infinite times, and it will be saved as vector<obj*> in cpp. 
	   When nodeCount==1, it will be saved as a single object in cpp.
	2. The attribute value startwith(i/f/s) identifies the type(int/float/string) of the attribute,
	   and the default value is s.
	3. The attribute value endwith 0, in xsd, it means that the attribute is optional, 
	   and the default value is required. 

## Test code
	test.cpp(with CMakeLists.txt CMakePresets.json) is the test code, 
	which is used to test the generated cpp file(output.hpp).
	test_in.xml is the xml file for test.cpp.

	when debug test.exe, copy test_in.xml to path of test.exe, eg:out\build\x64-Debug\ .

## generate_xsd.py
	generate_xsd.py is used to generate the xsd file corresponding to xml. 
	The xsd file can be used for xml file verification by tools such as xmlspy, 
	which is convenient for manual inspection of xml errors.

	Considering that the unique and key constraints of XML described by XSD are relatively complex, 
	the current script only generates a simplified version of XSD, 
	which is suitable for most simple application scenarios.