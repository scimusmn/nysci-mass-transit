import numpy as np
import math
import cv2 as cv



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


# invert bits
for k in dictionary:
  print(dictionary[k])
  # dictionary[k] = 0b1111111111111111111111111111111111111111111 ^ dictionary[k]
  dictionary[k] = 0b1111111111111111111111111111111111111111111 ^ dictionary[k]
  print(dictionary[k])
  print(bin(dictionary[k]))


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


def getValues(hsv, center, radius, num):
  tau = 2 * math.pi
  angles = [ i * tau / num for i in range(num) ]
  height, width, _ = hsv.shape
  cx, cy = int(round(center[0])), int(round(center[1]))
  values = []
  for angle in angles:
    x = round(cx + radius * math.cos(angle))
    y = round(cy + radius * math.sin(angle))
    x = clamp(int(x), 0, width-1)
    y = clamp(int(y), 0, height-1)
    values.append(hsv[y, x, 2])
  return values


def otsuVariance(values, threshold):
  return np.nansum([
    np.mean(mask) * np.var(values, where=mask)
    for mask in [ values >= threshold, values < threshold ]
  ])


def otsuThreshold(values):
  return min(
    range(np.min(values)+1, np.max(values)),
    key=lambda thresh: otsuVariance(values, thresh)
  )


def decode(hsv, p0, r, n=0, max_retries=32):
    values = getValues(hsv, p0, r, 43)

    # Compute threshold and binarize
    threshold = otsuThreshold(np.array(values))
    #threshold = 128                                      ############################### gotta try   
    binary = [1 if v > threshold else 0 for v in values]

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
