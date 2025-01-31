import numpy as np
import math
import cv2 as cv
import matplotlib.pyplot as pt
import sys 
import json





WINDOW_TITLE = 'window'


img = cv.imread("RunesBigHiRes.jpg")
#img = cv.imread("Rune1.png")
#img = cv.imread("img.jpg")


height, width = img.shape[0], img.shape[1]


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

  12:  0b0111011011100000110101110010101111111101010
  #13:  0b0010010000000111001110110001001000011101100,
  #14:  0b1101010100001101000101000111011011001111111,
  #15:  0b1101101111101110001110111101111111011000100
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


'''
def minimumDistance(x, y):
  minimumDistance = 43
 
  for i in range(42):
    distance = bin(x ^ y).count('1')
    if distance < minimumDistance:
        minimumDistance = distance

    x = bitRotate(x)
    
    #print(bin(x))
  return minimumDistance
'''
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
    dist = minimumDistance(x, int(dictionary[code]))

    print ("Hamming Distance: ", dist)
    if dist < 8: 
    #if dist < 8:                        ############ TONY CHANGED
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
    visualization_img = hsv.copy()

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
        cv.circle(visualization_img, (sx, sy), 2, (0, 255, 255), -1)  

    # Compute threshold and binarize
    min_val, max_val = min(sample_values), max(sample_values)
    #threshold = (float(max_val) + float(min_val)) / 2
    threshold = 128                                      ############################### gotta try   
    binary = [1 if v > threshold else 0 for v in sample_values]

    binary = [1 - b for b in binary]                   ########################  flip all bits
    
    #binary_str = ''.join(map(str, binary))
    binary_str = ''.join(map(str, binary[::-1]))   ##########################  REVERSED  

    print(binary_str)

    try:
        code = matchCode(int(binary_str, 2))  
    except ValueError:
        code = -1  

     # Convert to BGR for proper display
    visualization_img_bgr = cv.cvtColor(visualization_img, cv.COLOR_HSV2BGR)

    # Show the visualization
    cv.imshow('Sample Points Visualization', visualization_img_bgr)
    cv.waitKey(0)

    if code == -1 and n < max_retries:
        return decode(hsv, p0, r * 0.99, n + 1, max_retries)

    return code
   




class HoughParams:
  def __init__(self):
    self.minDist = 27
    self.cannyHigh = 100
    #self.threshold = 13
    self.threshold = 60

   # self.minRadius = 13
    self.minRadius = 100
    #self.maxRadius = 28
    self.maxRadius = 300


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
    
    return
    



  circles = np.uint16(np.around(circles))
  #print('drawing...')
  hsv = cv.cvtColor(dup, cv.COLOR_BGR2HSV)
  for i in circles[0,:]:
    cv.circle(drawImg, (i[0], i[1]), i[2], (0, 0, 255), 2)
    code = decode(hsv, i, i[2])

    #code = decode(grayImg, i, i[2])       ########### TONY TRIED GRAYSCALE INSTEAD

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


while True:
  
  findCircles(img, params, 4)
    

