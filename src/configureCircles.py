import cv2 as cv
from configuration import TransitConfiguration


config = TransitConfiguration()
# config.load('config.json')

camera = cv.VideoCapture(config.cameraId, cv.CAP_DSHOW)
camera.set(cv.CAP_PROP_FRAME_WIDTH, config.width)
camera.set(cv.CAP_PROP_FRAME_HEIGHT, config.height)

windowTitle = "Circles Configuration"
cv.namedWindow(windowTitle)

running = True
while running:
  _, img = camera.read()
  cv.imshow(windowTitle, img)
  if cv.waitKey(10) != -1:
    running = False

config.save('config.json')
