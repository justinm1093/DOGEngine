
#include "pch.h"
#include "XmlParseHelperData.h"

using namespace DOGEngine;
using namespace std;
using namespace glm;

RTTI_DEFINITIONS(XmlParseHelperData)

XmlParseHelperData::DatumTypeMap XmlParseHelperData::sDatumTypeMap;

const string XmlParseHelperData::sIntElement = "Int";
const string XmlParseHelperData::sFloatElement = "Float";
const string XmlParseHelperData::sStringElement = "String";
const string XmlParseHelperData::sVectorElement = "Vector";
const string XmlParseHelperData::sMatrixElement = "Matrix";

const string XmlParseHelperData::sNameAttribute = "name";
const string XmlParseHelperData::sValueAttribute = "value";
const string XmlParseHelperData::sIndexAttribute = "index";

XmlParseHelperData::XmlParseHelperData() :
	IXmlParseHelper()
{
	if(sDatumTypeMap.isEmpty())
	{
		sDatumTypeMap[sIntElement] = Datum::DatumType::Integer;
		sDatumTypeMap[sFloatElement] = Datum::DatumType::Float;
		sDatumTypeMap[sStringElement] = Datum::DatumType::String;
		sDatumTypeMap[sVectorElement] = Datum::DatumType::Vector;
		sDatumTypeMap[sMatrixElement] = Datum::DatumType::Matrix;
	}
}

//-----------------------------------------------------------------

XmlParseHelperData::~XmlParseHelperData()
{
}

//-----------------------------------------------------------------

IXmlParseHelper* XmlParseHelperData::clone()
{
	return new XmlParseHelperData();
}

//-----------------------------------------------------------------

bool XmlParseHelperData::startElementHandler(SharedData& sharedData, const string& name, const AttributeMap& attributes)
{
	bool result = false;

	SharedDataTable* sharedTable = sharedData.As<SharedDataTable>();
	if(sharedTable != nullptr)
	{
		Scope* scope = sharedTable->getScope();
		assert(scope != nullptr);

		if(sDatumTypeMap.containsKey(name))
		{
			// index attribute defined -- we set data in place
			if(attributes.containsKey(sIndexAttribute))
			{
				// we can handle this element -- requires "name" and "value"
				requiresAttribute(attributes, sNameAttribute, name, sharedTable->getXmlParseMaster()->getFileName());
				requiresAttribute(attributes, sValueAttribute, name, sharedTable->getXmlParseMaster()->getFileName());

				Datum& datum = (*scope)[attributes[sNameAttribute]];
				datum.setFromString(attributes[sValueAttribute], getIndexFromString(attributes[sIndexAttribute]));

				result = true;
			}

			// index attribute not defined -- we push back or set 0th element
			else
			{
				// we can handle this element -- requires "name" and "value"
				requiresAttribute(attributes, sNameAttribute, name, sharedTable->getXmlParseMaster()->getFileName());
				requiresAttribute(attributes, sValueAttribute, name, sharedTable->getXmlParseMaster()->getFileName());

				Datum& datum = (*scope)[attributes[sNameAttribute]];
				datum.setType(sDatumTypeMap[name]);

				datum.isExternal() ? datum.setFromString(attributes[sValueAttribute]) : datum.pushBackFromString(attributes[sValueAttribute]);

				result = true;
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool XmlParseHelperData::endElementHandler(SharedData& sharedData, const string& name)
{
	bool result = false;

	// expects a shared data table
	SharedDataTable* sharedTable = sharedData.As<SharedDataTable>();
	if(sharedTable != nullptr)
	{
		// we have a handler for this (doesn't matter which map we look at)
		if(sDatumTypeMap.containsKey(name))
		{
			result = true;
		}
	}

	return result;
}

//-----------------------------------------------------------------

void XmlParseHelperData::clearHandlerCache()
{
	sDatumTypeMap.clear();
}

//-----------------------------------------------------------------

int32_t XmlParseHelperData::getIndexFromString(const string& indexStr)
{
	int data;
	sscanf_s(indexStr.c_str(), "%d", &data);
	return data;
}
