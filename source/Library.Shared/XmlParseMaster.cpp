
#include "pch.h"
#include "XmlParseMaster.h"
#include "HashMap.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(XmlParseMaster)
RTTI_DEFINITIONS(SharedData)

const uint32_t XmlParseMaster::sBufferSize = 1024;

#pragma region XmlParseMaster

XmlParseMaster::XmlParseMaster(SharedData& data) :
	mParser(XML_ParserCreate(nullptr)),
	mSharedData(nullptr),
	mIsClone(false)
{
	if(mParser == nullptr)
	{
		throw exception("Error -- there was a problem creating the Expat parser object!");
	}

	setSharedData(data);
}

//-----------------------------------------------------------------

XmlParseMaster::~XmlParseMaster()
{
	// free the xml parser
	XML_ParserFree(mParser);

	// destroy helpers and shared data if we are a clone
	if(mIsClone)
	{
		if(mSharedData != nullptr)
		{
			delete mSharedData;
		}

		for(auto& helper : mHelpers)
		{
			delete helper;
		}
	}
}

//-----------------------------------------------------------------

XmlParseMaster* XmlParseMaster::clone()
{
	// create new parse master with a clone of our shared data (null or clone)
	XmlParseMaster* newMaster = new XmlParseMaster(*mSharedData->clone());
	newMaster->mIsClone = true;

	// clone our helpers
	for(auto& helper : mHelpers)
	{
		newMaster->mHelpers.pushBack(helper->clone());
	}

	return newMaster;
}

//-----------------------------------------------------------------

void XmlParseMaster::initialize()
{
	if(mParser != nullptr)
	{
		// reset state on parser
		XML_ParserReset(mParser, nullptr);

		// register element handlers
		XML_SetElementHandler(mParser, startElementHandler, endElementHandler);
		XML_SetCharacterDataHandler(mParser, charDataHandler);

		// set shared data that is passed around
		XML_SetUserData(mParser, mSharedData);
	}

	// init shared data and all registered helpers
	if(mSharedData != nullptr)
	{
		mSharedData->initialize();
	}

	for(auto& helper : mHelpers)
	{
		helper->initialize();
	}
}

//-----------------------------------------------------------------

bool XmlParseMaster::hasHelper(uint64_t typeID) const
{
	bool result = false;
	for(auto& helper : mHelpers)
	{
		if(helper->TypeIdInstance() == typeID)
		{
			result = true;
			break;
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool XmlParseMaster::addHelper(IXmlParseHelper& helper)
{
	bool didInsert = false;

	// only insert if we are not cloned and don't have helper of type
	if(!mIsClone)
	{
		if(!hasHelper(helper.TypeIdInstance()))
		{
			mHelpers.pushBack(&helper);
			didInsert = true;
		}
	}

	return didInsert;
}

//-----------------------------------------------------------------

bool XmlParseMaster::removeHelper(IXmlParseHelper& helper)
{
	// only remove if not clone, return if sucessful
	return !mIsClone && mHelpers.remove(&helper);
}

//-----------------------------------------------------------------

bool XmlParseMaster::parse(char* buffer, uint32_t length, bool isFinal, bool isFirstChunk)
{
	if(mSharedData == nullptr)
	{
		throw exception("Error -- cannot parse an Xml file with a null SharedData object!");
	}

	if(buffer == nullptr)
	{
		throw exception("Error -- cannot parse an Xml file with a null buffer!");
	}

	// we only initialize if the buffer is the first part of the data
	//		otherwise, we may have more data incoming later and don't want to blow away what we have now
	if(isFirstChunk)
	{
		initialize();
	}
	XML_Parse(mParser, buffer, length, isFinal);

	// successful parse if error code is none
	return XML_GetErrorCode(mParser) == XML_ERROR_NONE;
}

//-----------------------------------------------------------------

bool XmlParseMaster::parseFromFile(const string& fileName)
{
	bool result = false;

	if(mSharedData == nullptr)
	{
		throw exception("Error -- cannot parse an Xml file with a null SharedData object!");
	}

	ifstream stream(fileName, ios::in | ios::binary | ios::ate);
	int32_t fileSize = static_cast<int32_t>(stream.tellg());

	if(fileSize > 0)
	{
		// some file read initialization
		setFileName(fileName);
		char buffer[sBufferSize];
		bool isFirstChunk = true;

		// while there is file left to read in...
		stream.seekg(0, ios::beg);
		while(stream)
		{
			// read from file into buffer, break if nothing read (was at eof)
			stream.read(buffer, sBufferSize);
			if(stream.gcount() == 0)
			{
				break;
			}

			// parse this current chunk (have to give parse some bools saying where we are in the file)
			result = parse(buffer, static_cast<int32_t>(stream.gcount()), stream.eof(), isFirstChunk);
			isFirstChunk = false;

			// break if parse returned any error code
			if(!result)
			{
				break;
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------

void XmlParseMaster::setSharedData(SharedData& data)
{
	if(mIsClone)
	{
		delete mSharedData;
		mSharedData = nullptr;
	}

	// our old shared data no longer points to us
	if(mSharedData != nullptr)
	{
		mSharedData->mMaster = nullptr;
	}

	// detach new shared data from its previous master
	mSharedData = &data;
	mSharedData->setXmlParseMaster(*this);
}

//-----------------------------------------------------------------

void XmlParseMaster::setFileName(const string& fileName)
{
	mFileName = fileName;
}

//-----------------------------------------------------------------

const string& XmlParseMaster::getFileName() const
{
	return mFileName;
}

//-----------------------------------------------------------------

const Vector<IXmlParseHelper*>& XmlParseMaster::getHelpers() const
{
	return mHelpers;
}

//-----------------------------------------------------------------

SharedData* XmlParseMaster::getSharedData() const
{
	return mSharedData;
}

//-----------------------------------------------------------------

uint32_t XmlParseMaster::numHelpers() const
{
	return mHelpers.size();
}

//-----------------------------------------------------------------

bool XmlParseMaster::isClone() const
{
	return mIsClone;
}

//-----------------------------------------------------------------

void XmlParseMaster::startElementHandler(void* userData, const XML_Char* name, const XML_Char** attributes)
{
	assert(userData != nullptr);

	// add all attribute key-value pairs to map of strings as convenience to helpers
	HashMap<string, string> attributesMap;
	for(uint32_t i = 0; attributes[i] != nullptr; i += 2)
	{
		attributesMap[attributes[i]] = attributes[i + 1];
	}

	SharedData* sharedData = reinterpret_cast<SharedData*>(userData);
	sharedData->pushNewElement(name);
	sharedData->incrementDepth();

	// find a helper that will handle this element start
	for(auto& helper : sharedData->getXmlParseMaster()->mHelpers)
	{
		if(helper->startElementHandler(*sharedData, name, attributesMap))
		{
			break;
		}
	}
}

//-----------------------------------------------------------------

void XmlParseMaster::endElementHandler(void* userData, const XML_Char* name)
{
	assert(userData != nullptr);

	SharedData* sharedData = reinterpret_cast<SharedData*>(userData);
	sharedData->popElement(name);

	// find a helper that will handle this element end
	for(auto& helper : sharedData->getXmlParseMaster()->mHelpers)
	{
		if(helper->endElementHandler(*sharedData, name))
		{
			break;
		}
	}

	sharedData->decrementDepth();
}

//-----------------------------------------------------------------

void XmlParseMaster::charDataHandler(void* userData, const XML_Char* buffer, int length)
{
	assert(userData != nullptr);

	if(length > 0)
	{
		// ignore char data that is new lines characters or tabs
		if(buffer[0] != '\n' && buffer[0] != '\t')
		{
			SharedData* sharedData = reinterpret_cast<SharedData*>(userData);

			// find a helper that will handle this character data
			for(auto& helper : sharedData->getXmlParseMaster()->mHelpers)
			{
				if(helper->charDataHandler(*sharedData, buffer, length))
				{
					break;
				}
			}
		}
	}
}

#pragma endregion

//=================================================================

#pragma region SharedData

SharedData::SharedData() :
	mMaster(nullptr),
	mDepth(0),
	mElementStack()
{
}

//-----------------------------------------------------------------

SharedData::~SharedData()
{
}

//-----------------------------------------------------------------

void SharedData::initialize()
{
	mElementStack.clear();
	mDepth = 0;
}

//-----------------------------------------------------------------

void SharedData::incrementDepth()
{
	++mDepth;
}

//-----------------------------------------------------------------

void SharedData::decrementDepth()
{
	if(mDepth > 0)
	{
		--mDepth;
	}
}

//-----------------------------------------------------------------

void SharedData::pushNewElement(const string& name)
{
	mElementStack.pushFront(name);
}

//-----------------------------------------------------------------

void SharedData::popElement(const string& name)
{
	assert(!mElementStack.isEmpty());
	if(mElementStack.front() != name)
	{
		stringstream exceptionStr;
		exceptionStr <<
			"Error -- Element end tag does not match start tag in file " << mMaster->getFileName() <<
			" -- expected: " << mElementStack.front() << " -- actual: " << name;
		throw exception(exceptionStr.str().c_str());
	}

	mElementStack.popFront();
}

//-----------------------------------------------------------------

XmlParseMaster* SharedData::getXmlParseMaster() const
{
	return mMaster;
}

//-----------------------------------------------------------------

uint32_t SharedData::depth() const
{
	return mDepth;
}

//-----------------------------------------------------------------

void SharedData::setXmlParseMaster(XmlParseMaster& master)
{
	// our old master no longer points to us
	if(mMaster != nullptr)
	{
		mMaster->mSharedData = nullptr;
	}

	mMaster = &master;
}

#pragma endregion
