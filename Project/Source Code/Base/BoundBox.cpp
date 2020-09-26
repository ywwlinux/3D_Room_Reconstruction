
#include "BoundBox.h"

CBoundBox::CBoundBox()
{
	_xLeft = _xRight = _yBottom = _yTop = 0;
}

CBoundBox::CBoundBox(double xLeft, double yBottom, double xRight, double yTop)
{
	SetBoundBox(xLeft, yBottom, xRight, yTop);
}

CBoundBox::CBoundBox(const Vec2& leftmost, const Vec2& topmost)
{
	SetBoundBox(leftmost.x(), leftmost.y(), topmost.x(), topmost.y());
}

void CBoundBox::SetBoundBox(const Vec2& leftmost, const Vec2& topmost)
{
	SetBoundBox(leftmost.x(), leftmost.y(), topmost.x(), topmost.y());
}

void CBoundBox::SetBoundBox(double xmin, double ymin, double xmax, double ymax)
{
	if(xmin < xmax){
		_xLeft = xmin;
		_xRight = xmax;
	}
	else{
		_xLeft = xmax;
		_xRight = xmin;
	}

	if(ymin < ymax){
		_yBottom = ymin;
		_yTop= ymax;
	}
	else{
		_yBottom = ymax;
		_yTop= ymin;
	}

	return;
}

bool CBoundBox::IsIntersect(const CBoundBox& bbox) const
{
	if(_xLeft > bbox._xRight || _xRight < bbox._xLeft ||
		_yTop < bbox._yBottom || _yBottom > bbox._yTop){
			return false;
	}

	return true;
}

bool CBoundBox::IsPointInBBox(const Vec2& point) const
{
	double x = point.x(), y = point.y();
	if(x < (_xLeft - 1)){ // For numberical error
		return false;
	}
	else if(x > (_xRight + 1)){
		return false;
	}
	else if(y < (_yBottom - 1)){
		return false;
	}
	else if(y > (_yTop + 1)){
		return false;
	}
	else{
		return true;
	}
}

CBoundBox& CBoundBox::operator = (const CBoundBox& other)
{
	_xLeft = other._xLeft;
	_xRight = other._xRight;
	_yTop = other._yTop;
	_yBottom = other._yBottom;

	return *this;
}