
#include "Object.h"

Object::Object()
{
	_name = "No name";
	_bAlive = false;
	_bVisible = true;
	_iID = -1;
	_pParentObject = NULL;
	_iLayerLevel = 0;
}

Object::~Object()
{

}

void Object::SetName(const string& name)
{
	_name = name;
}

const string& Object::GetName() const
{
	return _name;
}

void Object::SetDescription(const string& description)
{
	_description = description;
}

const string& Object::GetDescription() const
{
	return _description;
}

void Object::SetAlive(bool bAlive)
{
	_bAlive = bAlive;
}

bool Object::GetAlive() const
{
	return _bAlive;
}

void Object::SetVisible(bool bVisible)
{
	_bVisible =bVisible;
}

bool Object::GetVisible() const
{
	return _bVisible;
}

void Object::SetID(long iID)
{
	_iID = iID;
}

long Object::GetID() const
{
	return _iID;
}

void Object::SetLayerLevel(long iLayerLevel)
{
	_iLayerLevel = iLayerLevel;
}

long Object::GetLayerLevel() const
{
	return _iLayerLevel;
}

void Object::SetParentObject(Object *pParentObject)
{
	_pParentObject = pParentObject;
}

Object* Object::GetParentObject()
{
	return _pParentObject;
}

const Object* Object::GetParentObject() const
{
	return _pParentObject;
}

void Object::Copy(const Object *other)
{
	_bAlive = other->_bAlive;
	_bVisible = other->_bVisible;
	_name = other->_name;
	_iID = other->_iID;
	_pParentObject = other->_pParentObject;
	_iLayerLevel = other->_iLayerLevel;
}
