/*******************************************************************
* Author	: wwyang
* Date		: 2007.4.19
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME CBoundBox -- bounding box for object

// .SECTION Description
// This class is for bounding box of object. The bounding box is used
// to describe the bounding rectangle area of embedded object 

#ifndef _BOUNDBOX_H_
#define _BOUNDBOX_H_

#include "Vec2.h"

class CBoundBox{
public:
	CBoundBox();
	CBoundBox(double xLeft, double yBottom, double xRight, double yTop);
	CBoundBox(const Vec2& leftmost, const Vec2& topmost);

	// Description:
	// Set the bounding box
	void SetBoundBox(double xmin, double ymin, double xmax, double ymax);
	void SetBoundBox(const Vec2& leftmost, const Vec2& topmost);

	// Description:
	// Get the bounding box
	double GetLeft() const
	{
		return _xLeft;
	}
	double GetRight() const
	{
		return _xRight;
	}
	double GetBottom() const
	{
		return _yBottom;
	}
	double GetTop() const
	{
		return _yTop;
	}
	inline void GetBoundBox(double& xmin, double& ymin, double& xmax, double& ymax) const;
	inline void GetBoundBox(Vec2& leftmost, Vec2& topmost) const;

	// Description:
	// Test if the bounding box is intersected with another
	bool IsIntersect(const CBoundBox& bbox) const;

	// Description:
	// Test if a point is in the box
	bool IsPointInBBox(const Vec2& point) const;

	CBoundBox& operator = (const CBoundBox& other);

private:
	double _xLeft, _xRight, _yBottom, _yTop;
};

inline void CBoundBox::GetBoundBox(double& xmin, double& ymin, double& xmax, double& ymax) const
{
	xmin = _xLeft; xmax = _xRight;
	ymin = _yBottom; ymax = _yTop;
}

inline void CBoundBox::GetBoundBox(Vec2& leftmost, Vec2& topmost) const
{
	leftmost.Set(_xLeft, _yBottom);
	topmost.Set(_xRight, _yTop);
}

#endif