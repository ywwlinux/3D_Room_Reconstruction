/*******************************************************************
* Author	: wwyang
* Date		: 2011.5.7
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME CElementObject - a class for representing each element component in a key frame

// .SECTION Description
// The object is a base base for the objects stored in a key frame
// We call the objects stored in a key frame as elements, which may be characters, backgrounds, and everything else that can be
// used to make animation such as path controllers, skeletons etc.

// .SECTION See also
// CCharacterObject CAnimationCharacterObject CFrameObjectManager 

#ifndef _ELEMENTOBJECT_H_
#define _ELEMENTOBJECT_H_

#include "../Base/Base.h"
#include "Object.h"

using namespace base;
class CElementObject : public Object{
public:
	enum E_Type{E_CHARACTER, E_IMAGE};
	CElementObject()
	{
		_elementType = E_CHARACTER;
	}
	virtual ~CElementObject(){}

	void SetElementType(E_Type type)
	{
		_elementType = type;
	}

	E_Type GetElementType() const
	{
		return _elementType;
	}

	void Copy(const Object *other)
	{
		Object::Copy(other);

		const CElementObject *pOtherObject = static_cast<const CElementObject *>(other);

		_elementType = pOtherObject->_elementType;
	}


protected:
	E_Type _elementType;
};

#endif