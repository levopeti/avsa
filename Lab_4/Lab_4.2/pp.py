import cv2
import numpy as np 


img = cv2.imread('dataset/bolt1/img/00000001.jpg')
print(img.shape)
cv2.imshow('window',img[:,:,0])
cv2.waitKey(0)