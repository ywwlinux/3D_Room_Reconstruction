/*******************************************************************
* Author	: wwyang
* Date		: 2017.11.23
* Copyright : Zhejiang Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CCameraAgent - An agent for the camera model

// .SECTION Description
// This class is for a camera model. 
// The camera is assumed to be a pinhole camera, i.e., no non-linear distortion occurs.

#ifndef _CAMERAAGENT_H_
#define _CAMERAAGENT_H_

#include "Object.h"
#include "ImageObject.h"
#include <vector>
#include <string>

class CCameraAgent : public Object {
public:
	CCameraAgent();
	virtual ~CCameraAgent();

	struct S_CameraIntrinsicParameter {
		double fx, fy; // focal lengths of the camera
		double skew; // usually it is zero
		double cx, cy; // coordinates of the principal point of the camera

		double kc[5]; // distortion parameters, usually kc[4] = .0; it's not used since the camera is assumed to be a pin-hole camera
	};

	struct S_CameraExtrinsicParameter {
		double tx, ty, tz; // translation
		double R[9]; // rotation
	};

	struct S_CameraCheckboardExtrinsicParameter {
		std::string buddyCamera; // the name of the camera that also take a picture for this checkerboard 
		double tx, ty, tz; // translation
		double R[9]; // rotation
	};

	// Description:
	// New itself
	virtual Object *New();

	// Description:
	// Set/get the intrinsic parameters for the camera
	void SetIntrinsicParameter(const S_CameraIntrinsicParameter &intrinsicParam);
	const S_CameraIntrinsicParameter& GetIntrinsicParameter() const;

	// Description:
	// Set/get the extrinic parameters for the camera
	void SetExtrinsicParameter(const S_CameraExtrinsicParameter &extrinsicParam);
	const S_CameraExtrinsicParameter& GetExtrinsicParameter() const;

	// Description:
	// Set/get the extrinsic parameters with respect to the checkboards
	void SetCheckboardExtrinsicParameter(const std::vector<S_CameraCheckboardExtrinsicParameter>& arrayExtrinsicParam);
	const std::vector<S_CameraCheckboardExtrinsicParameter>& GetCheckboardExtrinsicParameter() const;

	// Description:
	// Set/get the photo image captured by the camera
	void SetPhotoImage(CImageObject *pBkImg);
	const CImageObject* GetPhotoImage() const;
	CImageObject* GetPhotoImage();

private:
	S_CameraIntrinsicParameter _intrinsicParameter;
	S_CameraExtrinsicParameter _extrinsicParameter;

	std::vector<S_CameraCheckboardExtrinsicParameter> _arrayCheckboardExtrinsicParameter;

	CImageObject *_pCameraPhoto;
};


#endif // !_CAMERAAGENT_H_

