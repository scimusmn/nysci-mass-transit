import cv2 as cv
from configuration import TransitConfiguration
from collections import Counter
from decode import decode

#DEBUG = True
DEBUG = False

config = TransitConfiguration()
config.load('config.json')
if not DEBUG:
    config.communication.connect(5)


camera = config.openCamera()

window = "Detection"

tokens = [] 

class Position:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def dist2(self, other):
        dx = self.x - other.x
        dy = self.y - other.y
        return (dx*dx) + (dy*dy)

    def dist(self, other):
        return sqrt(self.dist2(other))

class History:
    def __init__(self, maxLen):
        self.list = []
        self.maxLen = maxLen

    def push(self, element):
        self.list.append(element)
        if len(self.list) > self.maxLen:
            self.list = self.list[1:]
        print(self.list)

    def getMajority(self):
        majority, _ = Counter(self.list).most_common(1)[0]
        return majority
        
class MatchToken:
    def __init__(self, type, x, y):
        self.matched = False
        self.type = type
        self.x = x
        self.y = y

class StabilizedToken:
    def __init__(self, x, y, type): 
        self.history = History(20)
        self.history.push(type)
        self.dist2 = 20**2
        self.pos = Position(x, y)
        self.matched = False

    def match(self, token):
        if self.matched:
            return
        if token.matched:
            return
        if self.pos.dist2(token) < self.dist2:
            token.matched = True
            self.matched = True
            self.history.push(token.type)

    def finalize(self):
        if not self.matched:
            self.history.push(-1)

    def getType(self):
        return self.history.getMajority()

    def tuple(self, width, height):
        return (1 - self.pos.x/width, 1 - self.pos.y/height, self.getType())
        

tokens = []


while True:
    for token in tokens:
        token.matched = False
    _, img = camera.read()
    img = config.reproject(img)
    circles = config.findCircles(img)
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    detected = []
    height, width, _ = img.shape
    for x, y, r in circles[0, :]:
        code = decode(hsv, (x, y), r)
        cv.circle(img, (x, y), round(0.94*r), (0, 255, 0), 2)
        cv.putText(img, str(code), (r+x, y), cv.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 4)
        if code != -1:
            detected.append(MatchToken(code, x, y))
    for token in tokens:
        for match in detected:
            token.match(match)
    tokens = [ t for t in tokens if t.getType() != -1 ]
    for token in tokens:
        token.finalize()
    for match in detected:
        if not match.matched:
            tokens.append(StabilizedToken(match.x, match.y, match.type))
    for token in tokens:
        pos = (round(token.pos.x), round(token.pos.y))
        cv.circle(img, pos, 40, (0, 0, 255), 2)
        cv.putText(img, str(token.getType()), pos, cv.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 4)
    if not DEBUG:
        config.sendTokens([ t.tuple(width, height) for t in tokens ])
    else:
        cv.imshow(window, img[::2, ::2])
    cv.waitKey(10)


camera.release()
config.communication.disconnect()
