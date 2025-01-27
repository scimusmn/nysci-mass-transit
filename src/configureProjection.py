import cv2
import numpy as np

MARK_RADIUS = 3
MARK_COLOR = (0, 255, 0)

def mouseCb(event, x, y, flags, userdata):
    points = userdata
    if event == cv2.EVENT_LBUTTONDOWN:
        # Left click
        points.append((x, y))
    elif event == cv2.EVENT_RBUTTONDOWN:
        # Right click
        if points:
            points.pop()

def getProjectionPoints(camera, window, points):
    ret, frame = camera.read()
    if not ret:
        return

    for point in points:
        cv2.circle(frame, point, MARK_RADIUS, MARK_COLOR, -1)

    caption = "Click on the "
    if len(points) == 0:
        caption += "UPPER LEFT "
    elif len(points) == 1:
        caption += "UPPER RIGHT "
    elif len(points) == 2:
        caption += "LOWER LEFT "
    elif len(points) == 3:
        caption += "LOWER RIGHT "
    else:
        caption += "????? "

    caption += "corner; right click to undo"
    cv2.putText(frame, caption, (10, frame.shape[0] - 10), cv2.FONT_HERSHEY_PLAIN, 1, (255, 0, 0))
    cv2.imshow(window, frame)
    cv2.waitKey(10)

def previewProjection(camera, window, projection):
    ret, frame = camera.read()
    if not ret:
        return False

    frame = cv2.warpPerspective(frame, projection, (frame.shape[1], frame.shape[0]))
    cv2.putText(frame, "Press any key to finish; right click to return to point selection", 
                (10, frame.shape[0] - 10), cv2.FONT_HERSHEY_PLAIN, 1, (255, 0, 0))
    cv2.imshow(window, frame)
    result = cv2.waitKey(10)
    if result != -1:
        return True
    else:
        return False
    
def loop(camera, window, points):
    while len(points) < 4:
        getProjectionPoints(camera, window, points)

    w = int(camera.get(cv2.CAP_PROP_FRAME_WIDTH))
    h = int(camera.get(cv2.CAP_PROP_FRAME_HEIGHT))

    expected = np.array([[0, 0], [w, 0], [0, h], [w, h]], dtype=np.float32)
    actual = np.array(points, dtype=np.float32)

    projection = cv2.getPerspectiveTransform(actual, expected)
    print(projection)

    while True:
        if previewProjection(camera, window, projection):
            return projection
        elif len(points) < 4:
            return loop(camera, window, points)


def configureProjection(config, camera): # perform image calibration
    points = []
    window = "Project surface"
    cv2.namedWindow(window, cv2.WINDOW_AUTOSIZE)
    cv2.setMouseCallback(window, mouseCb, points)

    # Get projection points
    config['projection'] = loop(camera, window, points)
    cv2.destroyWindow(window)
    
class Configuration(dict):
    pass

if __name__ == "__main__":
    config = Configuration()
    camera = cv2.VideoCapture(0)  # Change the index if another camera is used

    try:
        configureProjection(config, camera)
    finally:
        camera.release()
        cv2.destroyAllWindows()

    print("Projection Matrix:", config['projection'])
