import cv2
import mediapipe as mp
import speech_recognition as sr
import serial
import time

class handDetector():
    def __init__(self, mode=False, maxHands=2, detectionCon=1, trackCon=1):
        self.mode = mode
        self.maxHands = maxHands
        self.detectionCon = detectionCon
        self.trackCon = trackCon

        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(static_image_mode=False,
                      max_num_hands=2,
                      min_detection_confidence=0.5,
                      min_tracking_confidence=0.5)
        self.mpDraw = mp.solutions.drawing_utils

    def findHands(self, img, draw=True):
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        self.results = self.hands.process(imgRGB)
        # print(results.multi_hand_landmarks)

        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                    self.mpDraw.draw_landmarks(img, handLms, self.mpHands.HAND_CONNECTIONS)
        return img

    def findPosition(self, img, handNo=0, draw=True):

        lmlist = []
        if self.results.multi_hand_landmarks:
            myHand = self.results.multi_hand_landmarks[handNo]
            for id, lm in enumerate(myHand.landmark):
                h, w, c = img.shape
                cx, cy = int(lm.x * w), int(lm.y * h)
                lmlist.append([id, cx, cy])
                if draw:
                    cv2.circle(img, (cx, cy), 7, (255, 0, 255), cv2.FILLED)
        return lmlist
cap = cv2.VideoCapture(0) 
detector = handDetector()
arduino = serial.Serial(port='COM8')
recognizer = sr.Recognizer()
microphone = sr.Microphone()

try:
    print("A moment of silence, please...")

    with microphone as source:
        recognizer.adjust_for_ambient_noise(source)
    
    recognizer.energy_threshold += 20

    print("Set minimum energy threshold to {}".format(recognizer.energy_threshold))

    while True:
        print("Say something!")

        with microphone as source:
            audio = recognizer.listen(source, phrase_time_limit=3)
            print("Got it! Now to recognize it...")
            
        try:
            # recognize speech using Google Speech Recognition
            value = recognizer.recognize_google(audio)
            print("You said {}".format(value))
            # write value to arduino
            arduino.write(bytes(value, 'utf-8'))
            while True:
                success, img = cap.read()
                img = detector.findHands(img)
                lmlist = detector.findPosition(img)
                if len(lmlist) != 0:
                    while len(lmlist) != 0:
                        if lmlist[4][1] > 375:
                            print('right')
                            arduino.write(bytes("right", 'utf-8'))
                        elif lmlist[4][1] < 325:
                            print('left')
                            arduino.write(bytes("left", 'utf-8'))
                        time.sleep(1)
                        success, img = cap.read()
                        img = detector.findHands(img)
                        lmlist = detector.findPosition(img)
                    break
        except sr.UnknownValueError:
            print("Oops! Didn't catch that")
        except sr.RequestError as e:
            print("Uh oh! Couldn't request results from Google Speech Recognition service; {0}".format(e))
except KeyboardInterrupt:
    pass
