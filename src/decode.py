import numpy as np
import math
import cv2 as cv
<<<<<<< HEAD
import matplotlib.pyplot as pt
import sys 





WINDOW_TITLE = 'window'


cam = cv.VideoCapture(2, cv.CAP_DSHOW)
cam.set(cv.CAP_PROP_FRAME_WIDTH, 3840)
cam.set(cv.CAP_PROP_FRAME_HEIGHT, 2160)

ret, img = cam.read()
cam.release()
height, width = img.shape[0], img.shape[1]


=======
>>>>>>> tony



dictionary = {
  0:   0b1100111010111000101100011010001001110001001,
  1:   0b1000101011111000110001111000000110001011001,
  2:   0b1111101111111111011001010101001010010010101,
  3:   0b0011100001000111101100101100010000100111101,
  4:   0b1001100001100110000111001111011011100011110,
  5:   0b1101100110000000110010011011100101111010111,
  6:   0b0101111100100101000011101010010000101001000,
  7:   0b0100000101110100010010010010100110110001011,
  8:   0b0001101110000100000100010010001010011011000,
  9:   0b1001101011000101110011010001011110110010001,
  10:  0b0010111000010110000100111010100000110110101,
  11:  0b1101111010110101100110100011111010111110000,
  12:  0b0111011011100000110101110010101111111101010,
  13:  0b0010010000000111001110110001001000011101100,
  14:  0b1101010100001101000101000111011011001111111,
  15:  0b1101101111101110001110111101111111011000100,
}


<<<<<<< HEAD
=======
# invert bits
for k in dictionary:
  print(dictionary[k])
  # dictionary[k] = 0b1111111111111111111111111111111111111111111 ^ dictionary[k]
  dictionary[k] = 0b1111111111111111111111111111111111111111111 ^ dictionary[k]
  print(dictionary[k])
  print(bin(dictionary[k]))


>>>>>>> tony
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
  msb = x & (1 << 42)
  mask = (1 << 43) - 1
  shift = (x << 1) & mask
  return shift | (0 if msb == 0 else 1)

def bitCount(x):
  count = 0
  for i in range(43):
    if ((x >> i) & 1 == 0):
      count += 1
  return count

<<<<<<< HEAD
def minimumDistance(x, y):
  distance = bitCount(x ^ y)
  for i in range(42):
    x = bitRotate(x)
    distance = min(distance, bitCount(x ^ y))
  return distance

def matchCode(x):
  for code in dictionary:
    if (minimumDistance(x, dictionary[code]) < 8):
      return code
  return -1



def decode(hsv, p0, r, n=0):
  tau = 2 * 3.141592
  # hsv = cv.cvtColor(roi, cv.COLOR_BGR2HSV)
  sampleAngles = [ x * tau / 43 for x in range(43) ]
  samplePositions = list(map(lambda a : (round(r * math.cos(tau - a)), round(r * math.sin(tau - a))), sampleAngles))
  sampleValues = list(map(lambda p : hsv[clamp(int(p0[1])+p[1], 0, hsv.shape[1]), clamp(int(p0[0])+p[0], 0, hsv.shape[0]), 2], samplePositions))
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




class HoughParams:
  def __init__(self):
    self.minDist = 20
    self.cannyHigh = 100
    self.threshold = 24
    self.minRadius = 23
    self.maxRadius = 33

params = HoughParams()
def findCircles(img, params, scale=2):
  print('copying...')
  dup = img[::scale, ::scale]
  drawImg = dup.copy()
  print('converting...')
  grayImg = cv.cvtColor(dup, cv.COLOR_BGR2GRAY)
  print('blurring...')
  grayImg = cv.medianBlur(grayImg, 5)
  print('detecting...')
  circles = cv.HoughCircles(
    grayImg, cv.HOUGH_GRADIENT, 1, 
    params.minDist, param1=params.cannyHigh, param2=params.threshold, 
    minRadius=params.minRadius, maxRadius=params.maxRadius)
  circles = np.uint16(np.around(circles))
  print('drawing...')
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
  print('showing...')
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
  findCircles(img, params, 4)
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
=======


def minimumDistance(x, y):
  minimumDistance = 43
 
  for i in range(42):
    distance = bin(x ^ y).count('1')
    if distance < minimumDistance:
        minimumDistance = distance

    y = bitRotate(y)
    
    #print(bin(x))
  return minimumDistance

def matchCode(x):
  for code in dictionary:
    dist = minimumDistance(x, dictionary[code])
    #print(f"Checking code: {code}, Distance: {dist}, Threshold: 8")
    #if (minimumDistance(x, dictionary[code]) < 8):
 
    if dist < 8:                        ############  CHANGE  FOR  ACCURACY
      print("MATCHED! ", code )
      return code
  #print("NO MATCH!")
  return -1


def decode(hsv, p0, r, n=0, max_retries=32):
    tau = 2 * math.pi
    sample_count = 43  
    sample_angles = [x * tau / sample_count for x in range(sample_count)]

    # Convert center coordinates to integers
    cx, cy = int(round(p0[0])), int(round(p0[1]))

    # Create a copy for visualization
    #visualization_img = hsv.copy()

    sample_positions = []
    sample_values = []
    
    for angle in sample_angles:   
        # Compute sample points relative to the circle's center
        sx = int(round(cx + r * math.cos(angle)))
        sy = int(round(cy + r * math.sin(angle)))

        # Clamp values to stay inside the image bounds
        sx = clamp(sx, 0, hsv.shape[1] - 1)
        sy = clamp(sy, 0, hsv.shape[0] - 1)

        sample_positions.append((sx, sy))
        sample_values.append(hsv[sy, sx, 2])  # Sample brightness (V channel)

        # Draw sample points 
        #cv.circle(visualization_img, (sx, sy), 2, (0, 255, 255), -1)  

    # Compute threshold and binarize
    min_val, max_val = min(sample_values), max(sample_values)
    threshold = (float(max_val) + float(min_val)) / 2
    #threshold = 128                                      ############################### gotta try   
    binary = [1 if v > threshold else 0 for v in sample_values]

    # binary = [1 - b for b in binary]                   ########################  flip all bits
    
    #binary_str = ''.join(map(str, binary))
    binary_str = ''.join(map(str, binary[::-1]))   ##########################  REVERSED  

    #print(binary_str)

    try:
        code = matchCode(int(binary_str, 2))  
    except ValueError:
        code = -1  

     # Convert to BGR for proper display
    #visualization_img_bgr = cv.cvtColor(visualization_img, cv.COLOR_HSV2BGR)

    # Show the visualization
    #cv.imshow('Sample Points Visualization', visualization_img_bgr)
    #cv.waitKey(0)

    if code == -1 and n < max_retries:
        return decode(hsv, p0, r * 0.99, n + 1, max_retries)

    return code
>>>>>>> tony
