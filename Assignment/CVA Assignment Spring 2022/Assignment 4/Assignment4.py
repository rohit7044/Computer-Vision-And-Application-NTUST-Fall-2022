import numpy as np
import cv2
import glob
img = cv2.imread("toCalibrated.jpg")
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
#Get the 3d points of 3 squares
objp = np.array([[
    #Since all the square are equal and share the same Dimension
    [0,0,0],[30,0,0],[30,30,0],[0,30,0],
    [0,0,0],[30,0,0],[30,30,0],[0,30,0],
    [0,0,0],[30,0,0],[30,30,0],[0,30,0]
]])
#Get their corresponding location in 2d image
imgp = np.array([
    # First Square
    [[457,127]],[[473,322]],[[668,346]],[[674,136]],
    # Second Square
    [[452,417]],[[651,542]],[[592,592]],[[652,447]],
    # Third Square
    [[212,217]],[[264,457]],[[383,346]],[[356,143]]
])
#pre-processing for further calculations
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.
imgp = imgp.astype(np.float32)
objp = objp.astype(np.float32)

objpoints.append(objp)
imgpoints.append(imgp)
# Estimate the intrinsic parameter
camera_matrix = cv2.initCameraMatrix2D(objpoints,imgpoints, gray.shape[::-1])
# Estimate the rotation and translation vector
ret, matrix, distortion, r_vecs, t_vecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], camera_matrix, None,flags=cv2.CALIB_USE_EXTRINSIC_GUESS)
print("Intrinsic Parameter")
print(matrix)
print("\nRotation Matrix")
print(r_vecs)
print("\nTranslation Matrix")
print(t_vecs)