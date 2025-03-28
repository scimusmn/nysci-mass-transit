import numpy as np
import math
import cv2 as cv
import matplotlib.pyplot as pt
import sys 
import json





WINDOW_TITLE = 'window'


cam = cv.VideoCapture(1, cv.CAP_DSHOW)           # Change index for correct camera

width = int(cam.get(cv.CAP_PROP_FRAME_WIDTH))
height = int(cam.get(cv.CAP_PROP_FRAME_HEIGHT))
print("Camera resolution:", width, "x", height) 

cam.set(cv.CAP_PROP_FRAME_WIDTH, 3840)           
cam.set(cv.CAP_PROP_FRAME_HEIGHT, 2160)

width = int(cam.get(cv.CAP_PROP_FRAME_WIDTH))
height = int(cam.get(cv.CAP_PROP_FRAME_HEIGHT))
print("Camera resolution:", width, "x", height) 




ret, img = cam.read()
height, width = img.shape[0], img.shape[1]



def loadProjectionConfig(filename="camConfig.json"):
    try:
        with open(filename, "r") as file:
            config = json.load(file)
        # Convert lists back to NumPy arrays
        deserializedConfig = {
            key: (np.array(value) if isinstance(value, list) else value)
            for key, value in config.items()
        }
        print(f"Projection configuration loaded from {filename}.")
        return deserializedConfig
    except FileNotFoundError:
        print(f"No projection configuration file found.")
        return None


projectionConfig = loadProjectionConfig();
#print(projectionConfig)
 
#img = cv.warpPerspective(img, projectionConfig["projection"], (img.shape[1], img.shape[0]))


dictionary = {
  0:   0b11001110101110001011,
  1:   0b10001010111110001100,
  #2:   0b11111011111111110110,
  3:   0b00111000010001111011,
  4:   0b10011000011001100001,
  5:   0b11011001100000001100,
  6:   0b01011111001001010000,
  7:   0b01000001011101000100,
  8:   0b00011011100001000001,
  9:   0b10011010110001011100,
  10:  0b00101110000101100001,
  11:  0b11011110101101011001,
  12:  0b01110110111000001101,
  13:  0b00100100000001110011,
  14:  0b11010101000011010001,
  15:  0b11011011111011100011,
}


def clamp(x, lo, hi):
  if (x < lo):
    return lo
  elif (x > hi):
    return hi
  else:
    return x

def median(l):
  l = sorted(l)
  middle = round(len(l) / 2)
  if len(l) % 2 == 1:
    return l[middle]
  else:
    return 0.5 * (l[middle - 1] + l[middle])

def bitRotate(x):
  msb = x & (1 << 19)
  mask = (1 << 20) - 1
  shift = (x << 1) & mask
  return shift | (0 if msb == 0 else 1)

def bitCount(x):
  count = 0
  for i in range(20):
    if ((x >> i) & 1 == 0):
      count += 1
  return count

def minimumDistance(x, y):
  distance = bitCount(x ^ y)
  for i in range(19):
    x = bitRotate(x)
    distance = min(distance, bitCount(x ^ y))
  return distance

def matchCode(x):
  for code in dictionary:
    dist = minimumDistance(x, dictionary[code])
    #print(f"Checking code: {code}, Distance: {dist}, Threshold: 8")
    #if (minimumDistance(x, dictionary[code]) < 8):
    #if dist < 8:
    if dist < 2:
      print("MATCHED! ", code )
      return code
  #print("NO MATCH!")
  return -1


'''
def decode(hsv, p0, r, n=0):
  tau = 2 * 3.141592
  # hsv = cv.cvtColor(roi, cv.COLOR_BGR2HSV)
  sampleAngles = [ x * tau / 43 for x in range(43) ]
  samplePositions = list(map(lambda a : (round(r * math.cos(tau - a)), round(r * math.sin(tau - a))), sampleAngles))
  #sampleValues = list(map(lambda p : hsv[clamp(int(p0[1])+p[1], 0, hsv.shape[1]), clamp(int(p0[0])+p[0], 0, hsv.shape[0]), 2], samplePositions))  # Tony changed
  sampleValues = list(map(lambda p : hsv[clamp(int(p0[1])+p[1], 0, hsv.shape[0] - 1 ), clamp(int(p0[0])+p[0], 0, hsv.shape[1] - 1), 2], samplePositions))


  minVal = min(sampleValues)
  maxVal = max(sampleValues)
  m = (maxVal - minVal) / 2
  m += minVal
  binary = [ 1 if v > m else 0 for v in sampleValues ]
  binStr = ''.join(str(b) for b in binary)
  code = matchCode(int(binStr, 2))
  if (code == -1 and n < 16):
    # try again at a slightly smaller radius
    return decode(hsv, p0, 0.99*r, n+1)
  else:
    return code
    print(code, binStr, n)
    pt.plot(np.array(sampleAngles), np.array(sampleValues))
    pt.plot(np.array(sampleAngles), np.array([m] * len(sampleValues)))
    pt.plot(np.array(sampleAngles), np.array([ 255 * b for b in binary ]))
    pt.show()
'''

def decode(hsv, p0, r, n=0, max_retries=64):     # was 16
    """
    Decode a binary code from the circular region of an HSV image.

    Parameters:
        hsv (ndarray): HSV image array.
        p0 (tuple): Center point (x, y) of the circular region.
        r (float): Radius of the circular sampling region.
        n (int): Current retry count (default is 0).
        max_retries (int): Maximum number of retries (default is 16).

    Returns:
        int: Decoded value, or -1 if decoding fails.
    """

    tau = 2 * math.pi
    sample_count = 20  # Number of samples around the circle
    sample_angles = [x * tau / sample_count for x in range(sample_count)]

    # Calculate sample positions based on angles
    sample_positions = [
        (int(round(r * math.cos(a))), int(round(r * math.sin(a)))) for a in sample_angles
    ]

    # Sample the V (brightness) channel of the HSV image
    def clamp(value, min_val, max_val):
        """Clamp a value between min_val and max_val."""
        return max(min(value, max_val), min_val)

    sample_values = []
    for pos in sample_positions:

        # Cast to int before performing calculations
        raw_x = int(p0[0]) + int(pos[0])
        raw_y = int(p0[1]) + int(pos[1])


        # Safeguard against overflow/underflow in position calculation
        x = clamp(raw_x, 0, hsv.shape[1] - 1)  # Ensure within image width
        y = clamp(raw_y, 0, hsv.shape[0] - 1)  # Ensure within image height

        sample_values.append(hsv[y, x, 2])  # Sample V channel

    # Determine threshold and binarize values
    min_val, max_val = min(sample_values), max(sample_values)
    #threshold = (max_val + min_val) / 2     #################### OVERFLOW 
    threshold = (float(max_val) + float(min_val)) / 2

    binary = [1 if v > threshold else 0 for v in sample_values]

    # Convert binary array to integer code
    binary_str = ''.join(map(str, binary))
    try:
        code = matchCode(int(binary_str, 2))  # Call external matchCode function
    except ValueError:
        code = -1  # Handle invalid binary-to-int conversion

    # Retry if decoding fails and retries are available
    if code == -1 and n < max_retries:
        return decode(hsv, p0, r * 0.99, n + 1, max_retries)

  
    """
    pt.figure(figsize=(10, 6))
    pt.plot(sample_angles, sample_values, label="Sample Values")
    pt.axhline(y=threshold, color='r', linestyle='--', label="Threshold")
    pt.scatter(sample_angles, [255 * b for b in binary], color='g', label="Binary")
    pt.legend()
    pt.title("Decoding Visualization")
    pt.xlabel("Angle (radians)")
    pt.ylabel("Brightness (V channel)")
    pt.show()
    """
    return code




class HoughParams:
  def __init__(self):
    self.minDist = 27
    self.cannyHigh = 100
    self.threshold = 13
    self.minRadius = 13
    self.maxRadius = 28

params = HoughParams()

def findCircles(img, params, scale=2):
  #print('copying...')
  dup = img[::scale, ::scale]
  drawImg = dup.copy()
  #print('converting...')
  grayImg = cv.cvtColor(dup, cv.COLOR_BGR2GRAY)
  #print('blurring...')
  grayImg = cv.medianBlur(grayImg, 5)      # Try without this
  #print('detecting...')

  
  circles = cv.HoughCircles(
    grayImg, cv.HOUGH_GRADIENT, 1, 
    params.minDist, param1=params.cannyHigh, param2=params.threshold, 
    minRadius=params.minRadius, maxRadius=params.maxRadius)
  ''' 
  circles = cv.HoughCircles(       # trying different parameters    variation of HOUGH_GRADIENT to get better accuracy
    grayImg, cv.HOUGH_GRADIENT_ALT, 1, 
    params.minDist, param1=params.cannyHigh, param2=0.5, 
    minRadius=params.minRadius, maxRadius=params.maxRadius)
  ''' 
    # Check if circles are detected
  if circles is None:
    print("No circles detected.")
    #cv.imshow("Gray Image", grayImg)
    #cv.waitKey(0)
    return
    



  circles = np.uint16(np.around(circles))
  #print('drawing...')
  hsv = cv.cvtColor(dup, cv.COLOR_BGR2HSV)
  for i in circles[0,:]:
    cv.circle(drawImg, (i[0], i[1]), i[2], (0, 0, 255), 2)
    code = decode(hsv, i, i[2])
    cv.putText(drawImg, f"{code}", (i[0], i[1]), cv.FONT_HERSHEY_PLAIN, 1, (0, 0, 0), 4)
    cv.putText(drawImg, f"{code}", (i[0], i[1]), cv.FONT_HERSHEY_PLAIN, 1, (255, 255, 255), 2)
  cv.putText(drawImg, f"min dist: {params.minDist}", (10, 30), cv.FONT_HERSHEY_PLAIN, 1, (0, 255, 0))
  cv.putText(drawImg, f"canny: {params.cannyHigh}", (10, 40), cv.FONT_HERSHEY_PLAIN, 1, (0, 255, 0))
  cv.putText(drawImg, f"threshold: {params.threshold}", (10, 50), cv.FONT_HERSHEY_PLAIN, 1, (0, 255, 0))
  cv.putText(drawImg, f"radius [min]: {params.minRadius}", (10, 60), cv.FONT_HERSHEY_PLAIN, 1, (0, 255, 0))
  cv.putText(drawImg, f"radius [max]: {params.maxRadius}", (10, 70), cv.FONT_HERSHEY_PLAIN, 1, (0, 255, 0))
  #print('showing...')
  cv.imshow('detected circles', drawImg)
  k = cv.waitKey(1)
  if (k > 0):
    k = chr(k)
    match k:
      case 'D':
        params.minDist -= 1
      case 'd':
        params.minDist += 1
      case 'C':
        params.cannyHigh -= 1
      case 'c':
        params.cannyHigh += 1
      case 'T':
        params.threshold -= 1
      case 't':
        params.threshold += 1
      case '{':
        params.minRadius -= 1
      case '[':
        params.minRadius += 1
      case '}':
        params.maxRadius -= 1
      case ']':
        params.maxRadius += 1
      case _:
        return None
    findCircles(img, params, scale)



import timeit
#print(timeit.timeit("findCircles(img)"), globals=locals())
while True:
  ret, img = cam.read()
  img = cv.warpPerspective(img, projectionConfig["projection"], (img.shape[1], img.shape[0]))   ###### TONY ADDED
  findCircles(img, params, 4)
  #findCircles(img, params, 2)   #tony 



cam.release()
sys.exit(0)



x = 1500
def setX(v):
  global x
  x = v
y = 500
def setY(v):
  global y
  y = v
r = 500
def setR(v):
  global r
  r = v


def render():
  global x, y, r
  x = clamp(x, 256, width - 256)
  y = clamp(y, 256, height - 256)
  r = clamp(r, 20, 255)
  roi = img[y-256:y+256, x-256:x+256]
  draw = roi.copy()
  cv.circle(draw, (256, 256), r, (0, 255, 0), 2)
  cv.imshow(WINDOW_TITLE, draw)
  k = cv.waitKey(1)
  if (k >= 0):
    k = chr(k)
  if k == 'w':
    y -= 1
  elif k == 'W':
    y -= 10
  elif k == 'a':
    x -= 1
  elif k == 'A':
    x -= 10
  elif k == 's':
    y += 1
  elif k == 'S':
    y += 10
  elif k == 'd':
    x += 1
  elif k == 'D':
    x += 10
  elif k == ',':
    r -= 1
  elif k == '<':
    r -= 10
  elif k == '.':
    r += 1
  elif k == '>':
    r += 10
  elif k == ' ':
    decode(hsv, r)
    

if __name__ == '__main__':
    import timeit
    while 1:
        print(timeit.timeit("render()", globals=locals()))
