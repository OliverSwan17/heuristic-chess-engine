"""
This script seeks to take a screenshot of the chess board and translate 
it to a computer-readable object that C will understand. The functionality
that this will offer is to provide continuous updates as the game is 
played, delivering updates when something happens in game. I will 
probably end up sending information to the C program over localhost.

I'll probably develop two models. One will be template matching, the
other likely a CNN neural network via PyTorch.

If this works, hopefully we can develop it to actually perform the moves.
"""


import numpy as np       # Numpy
import cv2              # OpenCV
from PIL import Image   # Python Imaging Library
import torch            # Tensorflow

"""
Plan
    1. Image Processing
        a. Screenshot the window with Chess open
        b. Manipulate such that only the board is in frame 
        c. Apply image processing to make it easier for ML
    2. Machine Learning Model
        a. Have an empty 8x8 data structure which will get filled up. 
        b. i
    3. Send to C-process
        a. Probably make packet for the TCP/UDP bit stream
        b. Understand how Olli represents the board and send that
           over the wire.
"""
