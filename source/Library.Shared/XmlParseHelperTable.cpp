
#include "pch.h"
#include "XmlParseHelperTable.h"

#include "World.h"
#include "Sector.h"
#include "Entity.h"

#include "Action.h"
#include "ActionList.h"
#include "ActionListIf.h"
#include "ReactionAttributed.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(XmlParseHelperTable)

XmlParseHelperTable::HandlerMap XmlParseHelperTable::sHandlerMap;

const string XmlParseHelperTable::sScopeElement = "Scope";
const string XmlParseHelperTable::sWorldElement = "World";
const string XmlParseHelperTable::sSectorElement = "Sector";
const string XmlParseHelperTable::sEntityElement = "Entity";
const string XmlParseHelperTable::sActionElement = "Action";
const string XmlParseHelperTable::sReactionElement = "Reaction";

const string XmlParseHelperTable::sIfElement = "If";
const string XmlParseHelperTable::sThenElement = "Then";
const string XmlParseHelperTable::sElseElement = "Else";
const string XmlParseHelperTable::sConditionElement = "Condition";

const string XmlParseHelperTable::sNameAttribute = "name";
const string XmlParseHelperTable::sClassAttribute = "class";
const string XmlParseHelperTable::sValueAttribute = "value";
const string XmlParseHelperTable::sSubtypeAttribute = "subtype";

#pragma region Public Interface

XmlParseHelperTable::XmlParseHelperTable() :
	IXmlParseHelper()
{
	// populate the handler map
	if(sHandlerMap.isEmpty())
	{
		sHandlerMap[sScopeElement] = &XmlParseHelperTable::elementHandlerScope;
		sHandlerMap[sWorldElement] = &XmlParseHelperTable::elementHandlerWorld;
		sHandlerMap[sSectorElement] = &XmlParseHelperTable::elementHandlerSector;
		sHandlerMap[sEntityElement] = &XmlParseHelperTable::elementHandlerEntity;
		sHandlerMap[sActionElement] = &XmlParseHelperTable::elementHandlerAction;
		sHandlerMap[sReactionElement] = &XmlParseHelperTable::elementHandlerReaction;

		sHandlerMap[sIfElement] = &XmlParseHelperTable::elementHandlerIf;
		sHandlerMap[sThenElement] = &XmlParseHelperTable::elementHandlerThen;
		sHandlerMap[sElseElement] = &XmlParseHelperTable::elementHandlerElse;
		sHandlerMap[sConditionElement] = &XmlParseHelperTable::elementHandlerCondition;
	}
}

//-----------------------------------------------------------------

XmlParseHelperTable::~XmlParseHelperTable()
{
}

//-----------------------------------------------------------------

XmlParseHelperTable* XmlParseHelperTable::clone()
{
	return new XmlParseHelperTable();
}

//-----------------------------------------------------------------

bool XmlParseHelperTable::startElementHandler(SharedData& sharedData, const string& name, const AttributeMap& attributes)
{
	bool result = false;

	SharedDataTable* sharedTable = sharedData.As<SharedDataTable>();
	if(sharedTable != nullptr)
	{
		// invoke the handler method for this element name
		if(sHandlerMap.containsKey(name))
		{
			sHandlerMap[name](*this, *sharedTable, attributes);
			result = true;
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool XmlParseHelperTable::endElementHandler(SharedData& sharedData, const string& name)
{
	bool result = false;

	SharedDataTable* sharedTable = sharedData.As<SharedDataTable>();
	if(sharedTable != nullptr)
	{
		// handled if we have a handler method for this tag name
		if(sHandlerMap.containsKey(name))
		{
			result = true;
			if(name != sConditionElement)	// TODO -- Condition will eventually be an expression???
			{
				sharedTable->setScopeToParent();
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------

void XmlParseHelperTable::clearHandlerCache()
{
	sHandlerMap.clear();
}

#pragma endregion

//-----------------------------------------------------------------

#pragma region Element Handlers

void XmlParseHelperTable::elementHandlerScope(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// basic Scopes require no parent type validation
	requiresAttribute(attributes, sNameAttribute, sScopeElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Scope* scope = new Scope();
	sharedData.setScope(attributes[sNameAttribute], *scope);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerWorld(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// World must exist as the root
	assert(sharedData.getScope() == nullptr);
	assert(sharedData.depth() == 1);

	requiresAttribute(attributes, sNameAttribute, sWorldElement, sharedData.getXmlParseMaster()->getFileName(), true);

	World* world = new World(attributes[sNameAttribute]);
	sharedData.setScope(attributes[sNameAttribute], *world);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerSector(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Sector is root of subfile or child of World
	assert(sharedData.getScope() == nullptr || sharedData.getScope()->Is(World::TypeIdClass()));

	requiresAttribute(attributes, sNameAttribute, sSectorElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Sector* sector = new Sector(attributes[sNameAttribute]);
	sharedData.setScope(World::sSectorsAttribute, *sector);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerEntity(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Entity is root of subfile or child of Sector
	assert(sharedData.getScope() == nullptr || sharedData.getScope()->Is(Sector::TypeIdClass()));

	requiresAttribute(attributes, sClassAttribute, sEntityElement, sharedData.getXmlParseMaster()->getFileName(), true);
	requiresAttribute(attributes, sNameAttribute, sEntityElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Entity* entity = Factory<Entity>::create(attributes[sClassAttribute]);
	assert(entity != nullptr);

	entity->setName(attributes[sNameAttribute]);
	sharedData.setScope(Sector::sEntitiesAttribute, *entity);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerAction(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Action is root of own file or child of World / Sector / Entity / ActionList
	assert(sharedData.getScope() == nullptr || 
		sharedData.getScope()->Is(World::TypeIdClass()) || sharedData.getScope()->Is(Sector::TypeIdClass()) ||
		sharedData.getScope()->Is(Entity::TypeIdClass()) || sharedData.getScope()->Is(ActionList::TypeIdClass()));

	requiresAttribute(attributes, sClassAttribute, sActionElement, sharedData.getXmlParseMaster()->getFileName(), true);
	requiresAttribute(attributes, sNameAttribute, sActionElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Action* action = Factory<Action>::create(attributes[sClassAttribute]);
	assert(action != nullptr);

	action->setName(attributes[sNameAttribute]);
	sharedData.setScope(Entity::sActionsAttribute, *action);

	// add any remaining key-value pairs from the element
	addAdditionalAttributes(sharedData, attributes);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerReaction(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Reaction is root of own file or child of World / Sector / Entity / Action
	assert(sharedData.getScope() == nullptr ||
		sharedData.getScope()->Is(World::TypeIdClass()) || sharedData.getScope()->Is(Sector::TypeIdClass()) ||
		sharedData.getScope()->Is(Entity::TypeIdClass()) || sharedData.getScope()->Is(Action::TypeIdClass()));

	requiresAttribute(attributes, sNameAttribute, sReactionElement, sharedData.getXmlParseMaster()->getFileName(), true);
	requiresAttribute(attributes, sSubtypeAttribute, sReactionElement, sharedData.getXmlParseMaster()->getFileName(), true);

	ReactionAttributed* reaction = new ReactionAttributed(attributes[sNameAttribute]);
	reaction->addSubtype(attributes[sSubtypeAttribute]);

	sharedData.setScope(Entity::sReactionsAttribute, *reaction);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerIf(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// ActionListIf is root of own file or child of World / Sector / Entity / ActionList
	assert(sharedData.getScope() == nullptr ||
		sharedData.getScope()->Is(World::TypeIdClass()) || sharedData.getScope()->Is(Sector::TypeIdClass()) ||
		sharedData.getScope()->Is(Entity::TypeIdClass()) || sharedData.getScope()->Is(ActionList::TypeIdClass()));

	requiresAttribute(attributes, sNameAttribute, sIfElement, sharedData.getXmlParseMaster()->getFileName(), true);

	ActionListIf* actionIf = new ActionListIf(attributes[sNameAttribute]);
	sharedData.setScope(Entity::sActionsAttribute, *actionIf);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerThen(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Then is child of ActionListIf
	assert(sharedData.getScope() != nullptr && sharedData.getScope()->Is(ActionListIf::TypeIdClass()));

	requiresAttribute(attributes, sNameAttribute, sThenElement, sharedData.getXmlParseMaster()->getFileName(), true);
	requiresAttribute(attributes, sClassAttribute, sThenElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Action* action = static_cast<ActionListIf*>(sharedData.getScope())->createThenBlock(attributes[sClassAttribute], attributes[sNameAttribute]);
	assert(action != nullptr);

	sharedData.setScope(ActionListIf::sThenAttribute, *action);

	// add any remaining key-value pairs from the element
	addAdditionalAttributes(sharedData, attributes);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerElse(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// Else is child of ActionListIf
	assert(sharedData.getScope() != nullptr && sharedData.getScope()->Is(ActionListIf::TypeIdClass()));

	requiresAttribute(attributes, sNameAttribute, sElseElement, sharedData.getXmlParseMaster()->getFileName(), true);
	requiresAttribute(attributes, sClassAttribute, sElseElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Action* action = static_cast<ActionListIf*>(sharedData.getScope())->createElseBlock(attributes[sClassAttribute], attributes[sNameAttribute]);
	assert(action != nullptr);

	sharedData.setScope(ActionListIf::sElseAttribute, *action);

	// add any remaining key-value pairs from the element
	addAdditionalAttributes(sharedData, attributes);
}

//-----------------------------------------------------------------

void XmlParseHelperTable::elementHandlerCondition(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	// TODO -- Condition will be expression???

	// Condition is child of ActionListIf
	assert(sharedData.getScope() != nullptr && sharedData.getScope()->Is(ActionListIf::TypeIdClass()));

	requiresAttribute(attributes, sValueAttribute, sConditionElement, sharedData.getXmlParseMaster()->getFileName(), true);

	Datum& datum = (*sharedData.getScope())[ActionListIf::sConditionAttribute];
	datum.setType(Datum::DatumType::Integer);

	datum.isExternal() ? datum.setFromString(attributes[sValueAttribute]) : datum.pushBackFromString(attributes[sValueAttribute]);
}

#pragma endregion

//-----------------------------------------------------------------

#pragma region Parse Utility

void XmlParseHelperTable::addAdditionalAttributes(SharedDataTable& sharedData, const AttributeMap& attributes)
{
	for(auto& kvPair : attributes)
	{
		string& name = kvPair.first;
		string& value = kvPair.second;
		if(name == sNameAttribute || name == sClassAttribute)
		{
			continue;
		}

		Datum& datum = (*sharedData.getScope())[name];
		datum.setType(Datum::DatumType::String);

		datum.isExternal() ? datum.setFromString(value) : datum.pushBackFromString(value);
	}
}

#pragma endregion
