import cv2 as cv
from configuration import TransitConfiguration


config = TransitConfiguration()
config.load('config.json')

camera = config.openCamera()

window = "Circles Configuration"
cv.namedWindow(window)

# create trackbars
def minDistSlider(value):
  config.circleParams.minDist = value
cv.createTrackbar("minDist", window, config.circleParams.minDist, 500, minDistSlider)
def param1Slider(value):
  config.circleParams.param1 = max(value, 1)
cv.createTrackbar("param1", window, config.circleParams.param1, 500, param1Slider)
def param2Slider(value):
  config.circleParams.param2 = value
cv.createTrackbar("param2", window, config.circleParams.param2, 500, param2Slider)
def minRadiusSlider(value):
  config.circleParams.minRadius = value
cv.createTrackbar("minRadius", window, config.circleParams.minRadius, 500, minRadiusSlider)
def maxRadiusSlider(value):
  config.circleParams.maxRadius = value
cv.createTrackbar("maxRadius", window, config.circleParams.maxRadius, 500, maxRadiusSlider)


running = True
while running:
  _, img = camera.read()
  img = config.reproject(img)
  circles = config.findCircles(img)
  for x, y, r in circles[0,:]:
    cv.circle(img, (x, y), r, (0, 0, 255), 2)
  cv.imshow(window, img)
  if cv.waitKey(10) != -1:
    running = False

config.save('config.json')
