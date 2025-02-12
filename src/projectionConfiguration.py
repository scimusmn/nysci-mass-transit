import numpy as np
import cv2 as cv
from configuration import TransitConfiguration


def mouseCallback(event, x, y, flags, points):
  if event == cv.EVENT_LBUTTONDOWN:
    if len(points) < 4:
      points.append((x, y))
  elif event == cv.EVENT_RBUTTONDOWN:
    if len(points) > 0:
      points.pop()


config = TransitConfiguration()
config.load('config.json')
W = config.physicalWidth * config.dpi
H = config.physicalHeight * config.dpi


camera = cv.VideoCapture(config.cameraId, cv.CAP_DSHOW)
camera.set(cv.CAP_PROP_FRAME_WIDTH, config.width)
camera.set(cv.CAP_PROP_FRAME_HEIGHT, config.height)


window = "Projection Configuration"
cv.namedWindow(window)
points = []
cv.setMouseCallback(window, mouseCallback, points)


def gatherPoints(window, points):
  while len(points) < 4:
    caption = "Click the "
    if len(points) == 0:
      caption += "upper left"
    elif len(points) == 1:
      caption += "upper right"
    elif len(points) == 2:
      caption += "lower left"
    elif len(points) == 3:
      caption += "lower right"
    caption += " corner; right click to undo"

    _, img = camera.read()
    for point in points:
      cv.circle(img, point, 3, (255, 0, 0), -1)
    cv.putText(img, caption, (10, 10), cv.FONT_HERSHEY_PLAIN, 1, (255, 0, 0))
    cv.imshow(window, img)
    cv.waitKey(10)


def previewProjection(window, points, projection):
  while len(points) == 4:
    _, img = camera.read()
    img = cv.warpPerspective(img, projection, (W, H))
    cv.putText(
      img, "Press any key to finish; right click to return to point selection", 
      (10, img.shape[0] - 10), cv.FONT_HERSHEY_PLAIN, 1, (255, 0, 0)
    )
    cv.imshow(window, img)
    if cv.waitKey(10) != -1:
      return True
  return False


def loop(window, points):
  gatherPoints(window, points)

  # compute projection
  expected = np.array([[0, 0], [W, 0], [0, H], [W, H]], dtype=np.float32)
  actual = np.array(points, dtype=np.float32)
  config.projection = cv.getPerspectiveTransform(actual, expected)
  a = config.projection.tolist()
  print(a, type(a))
  if not previewProjection(window, points, config.projection):
    loop(window, points)
  

loop(window, points)

camera.release()
config.save('config.json')
