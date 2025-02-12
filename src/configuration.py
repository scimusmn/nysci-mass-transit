import numpy as np
import cv2 as cv
import json


class HoughCircleParams:
  def __init__(self):
    self.dp = 1.5
    self.minDist = 100
    self.param1 = 100
    self.param2 = 100
    self.minRadius = 20
    self.maxRadius = 50

  def findCircles(self, img):
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    cv.medianBlur(gray, 5)
    circles = cv.HoughCircles(
      gray, cv.HOUGH_GRADIENT, 
      self.dp, self.minDist,
      param1=self.param1, param2=self.param2, 
      minRadius=self.minRadius, maxRadius=self.maxRadius
    )
    if circles is not None:
      circles = np.uint16(circles)
      return circles
    else:
      return np.zeros((1, 0), dtype=np.uint16)


  def toDict(self):
    return {
      "dp": self.dp,
      "minDist": self.minDist,
      "param1": self.param1,
      "param2": self.param2,
      "minRadius": self.minRadius,
      "maxRadius": self.maxRadius,
    }

  def fromDict(self, data):
    self.dp = data["dp"]
    self.minDist = data["minDist"]
    self.param1 = data["param1"]
    self.param2 = data["param2"]
    self.minRadius = data["minRadius"]
    self.maxRadius = data["maxRadius"]




class TransitConfiguration:
  def __init__(self):
    self.cameraId = 0
    self.width = 640
    self.height = 480
    self.physicalWidth = 40
    self.physicalHeight = 31
    self.dpi = 20
    self.projection = np.eye(3, 3)
    self.circleParams = HoughCircleParams()

  def reproject(self, img):
    W = self.physicalWidth * self.dpi
    H = self.physicalHeight * self.dpi
    return cv.warpPerspective(img, self.projection, (W, H))

  def findCircles(self, img):
    return self.circleParams.findCircles(img)

  def load(self, filename):
    with open(filename) as file:
      data = json.load(file)
      self.cameraId = data["cameraId"]
      self.width = data["width"]
      self.height = data["height"]
      self.physicalWidth = data["physicalWidth"]
      self.physicalHeight = data["physicalHeight"]
      self.dpi = data["dpi"]
      self.projection = np.array(data["projection"])   
      self.circleParams.fromDict(data["circleParams"])


  def save(self, filename):
    with open(filename, 'w') as file:
      data = {
        "cameraId": self.cameraId,
        "width": self.width,
        "height": self.height,
        "physicalWidth": self.physicalWidth,
        "physicalHeight": self.physicalHeight,
        "dpi": self.dpi,
        "projection": self.projection.tolist(),
        "circleParams": self.circleParams.toDict(),
      }
      json.dump(data, file)
