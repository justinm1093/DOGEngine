
#include "pch.h"
#include "XmlParseHelperSubfile.h"

#include "World.h"
#include "Sector.h"
#include "Entity.h"
#include "Action.h"
#include "ActionList.h"
#include "Reaction.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(XmlParseHelperSubfile)

const string XmlParseHelperSubfile::sFileElement = "File";
const string XmlParseHelperSubfile::sPathAttribute = "path";
const string XmlParseHelperSubfile::sScopeName = "scope";

XmlParseHelperSubfile::XmlParseHelperSubfile() :
	IXmlParseHelper()
{
}

//-----------------------------------------------------------------

XmlParseHelperSubfile::~XmlParseHelperSubfile()
{
}

//-----------------------------------------------------------------

IXmlParseHelper* XmlParseHelperSubfile::clone()
{
	return new XmlParseHelperSubfile();
}

//-----------------------------------------------------------------

bool XmlParseHelperSubfile::startElementHandler(SharedData& sharedData, const std::string& name, const AttributeMap& attributes)
{
	bool result = false;

	SharedDataTable* sharedTable = sharedData.As<SharedDataTable>();
	if(sharedTable != nullptr)
	{
		if(name == sFileElement)
		{
			Scope* currentScope = sharedTable->getScope();
			assert(currentScope != nullptr && sharedTable->depth() > 1);

			requiresAttribute(attributes, sPathAttribute, sFileElement, sharedData.getXmlParseMaster()->getFileName(), true);

			unique_ptr<XmlParseMaster> masterClone(sharedTable->getXmlParseMaster()->clone());

			// build path to subfile
			string subFilePath = sharedTable->getXmlParseMaster()->getFileName();
			size_t index = subFilePath.find_last_of('/');
			subFilePath.erase(index + 1);
			subFilePath.append(attributes[sPathAttribute]);

			// parse the subfile, validate that it returned stuff
			masterClone->parseFromFile(subFilePath);
			Scope* subFileScope = masterClone->getSharedData()->As<SharedDataTable>()->extractScope();
			assert(subFileScope != nullptr && !subFileScope->Is(World::TypeIdClass()));

			// subfile was a sector -- we need world at this level
			if(subFileScope->Is(Sector::TypeIdClass()))
			{
				assert(currentScope->Is(World::TypeIdClass()));
				currentScope->adopt(World::sSectorsAttribute, *subFileScope);
			}

			// subfile was an entity -- we need a sector at this level
			else if(subFileScope->Is(Entity::TypeIdClass()))
			{
				assert(currentScope->Is(Sector::TypeIdClass()));
				currentScope->adopt(Sector::sEntitiesAttribute, *subFileScope);
			}

			// subfile was a reaction
			else if(subFileScope->Is(Reaction::TypeIdClass()))
			{
				assert(
					currentScope->Is(World::TypeIdClass()) || currentScope->Is(Sector::TypeIdClass()) ||
					currentScope->Is(Entity::TypeIdClass()) || currentScope->Is(Action::TypeIdClass()));
				currentScope->adopt(Entity::sReactionsAttribute, *subFileScope);
			}

			// subfile was any other type of action
			else if(subFileScope->Is(Action::TypeIdClass()))
			{
				assert(
					currentScope->Is(World::TypeIdClass()) || currentScope->Is(Sector::TypeIdClass()) || 
					currentScope->Is(Entity::TypeIdClass()) || currentScope->Is(ActionList::TypeIdClass()));
				currentScope->adopt(Entity::sActionsAttribute, *subFileScope);
			}

			// subfile was a basic type -- we just adopt it
			else
			{
				currentScope->adopt(sScopeName, *subFileScope);
			}

			result = true;
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool XmlParseHelperSubfile::endElementHandler(SharedData& sharedData, const std::string& name)
{
	UNREFERENCED_PARAMETER(sharedData);
	return name == sFileElement;
}
