/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.23
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME ObjectManager - Abstract class to manage objects of some kind

// .SECTION Description
// This class is an abstract class which is used to manage some
// kind of objects

// .SECTION See also
// CObjectManagerFactory

#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

class CObjectManager{
public:
	virtual ~CObjectManager(){}

	// Description:
	// Destroy the manager -- free all memory
	virtual void DestroyManager() = 0;
};

#endif