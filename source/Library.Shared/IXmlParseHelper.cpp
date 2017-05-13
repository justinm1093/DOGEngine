
#include "pch.h"
#include "IXmlParseHelper.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(IXmlParseHelper)

void IXmlParseHelper::initialize()
{
}

//-----------------------------------------------------------------

bool IXmlParseHelper::charDataHandler(SharedData& sharedData, const char* buffer, int32_t length)
{
	UNREFERENCED_PARAMETER(sharedData);
	UNREFERENCED_PARAMETER(buffer);
	UNREFERENCED_PARAMETER(length);
	return false;
}

//-----------------------------------------------------------------

void IXmlParseHelper::requiresAttribute(const AttributeMap& attributes, const string& attributeName, const string& elementName, const string& fileName, bool requiresValue)
{
	// attributes does not have expected attribute name -- throw exception
	if(!attributes.containsKey(attributeName))
	{
		stringstream exceptionStr;
		exceptionStr << "Error -- Element " << elementName << " in file " << fileName << " does not have required attribute " << attributeName;
		throw exception(exceptionStr.str().c_str());
	}

	// if the attribute requires a value (non-empty string) and there is none -- throw exception
	if(requiresValue)
	{
		if(attributes[attributeName] == "")
		{
			stringstream exceptionStr;
			exceptionStr << "Error -- Element " << elementName << " in file " << fileName << " does not set value on required attribute " << attributeName;
			throw exception(exceptionStr.str().c_str());
		}
	}
}
