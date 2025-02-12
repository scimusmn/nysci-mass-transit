import cv2 as cv
from configuration import TransitConfiguration
from decode import decode

DEBUG = True

config = TransitConfiguration()
config.load('config.json')
config.communication.connect(5)


camera = config.openCamera()

window = "Detection"

tokens = [] 

while True:
    _, img = camera.read()
    img = config.reproject(img)
    circles = config.findCircles(img)
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    tokens = []
    height, width, _ = img.shape
    for x, y, r in circles[0, :]:
        code = decode(hsv, (x, y), r)
        cv.circle(img, (x, y), round(0.94*r), (0, 255, 0), 2)
        cv.putText(img, str(code), (x, y), cv.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 2)
        if code != -1:
            tokens.append((x/width, y/height, code))
    config.sendTokens(tokens)
    cv.imshow(window, img)
    cv.waitKey(10)


camera.release()
config.communication.disconnect()
