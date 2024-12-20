#!/usr/bin/env python3

"""
This script seeks to take a screenshot of the chess board and translate 
it to a computer-readable object that C will understand. The functionality
that this will offer is to provide continuous updates as the game is 
played, delivering updates when something happens in game. I will 
probably end up sending information to the C program over localhost.
"""

import os               
import threading        
import time
import hashlib
import numpy as np      
import pyautogui        
import re
from matplotlib import pyplot as plt
import cv2          
from PIL import Image   
import torch            
from torch import nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
from torchvision.io import decode_image

DIR_PATH = os.path.dirname(os.path.realpath(__file__))
IMAGES_PATH = DIR_PATH + '/images/'
TRAINING_PATH = DIR_PATH + '/training_data/'
screenshot_count = 0
hashes = set()


class ChessboardSquares():
    def __init__(self):
        self.__labels_map__ = {
                               0: 'lbr',
                               1: 'dbk',
                               2: 'lbb',
                               3: 'dbq',
                               4: 'lbk',
                               5: 'dbb',
                               6: 'lbk',
                               7: 'dbr',
                               8: 'dbp',
                               9: 'lbp',
                               10: 'due',
                               11: 'lue',
                               12: 'dwp',
                               13: 'lwp',
                               14: 'dwr',
                               15: 'lwk',
                               16: 'dwb',
                               17: 'lwq',
                               18: 'dwk',
                               19: 'lwb',
                               20: 'dwk',
                               21: 'lwr'}
        self.label_to_index = {label: idx for idx, label in self.__labels_map__.items()}
        self.transform = transforms.Compose([transforms.Resize(size=( 64, 64)), transforms.ToTensor()])
        self.dataPIL = self.get_files_PIL()
        self.dataTensor = self.get_files_tensor()
        self.len = self.__len__()

    def __len__(self):
        return len(self.dataPIL)

    def __getitem__(self, idx):
       # return self.dataTensor[idx][0], self.dataTensor[idx][1] # return data, label 
        img, label = self.dataTensor[idx]
        label_index = self.label_to_index[label]
        return img, label_index


    # List of (raw_data, type)
    def get_files_PIL(self): 
        out = []

        for file in os.listdir(TRAINING_PATH):
            key = -1
            file_match = re.search(r"DATA_\d+_(\d+)\.png", file)
            key = file_match.group(1)
            if key == -1:
                raise ValueError(r"Match error: {file}")
            out.append( (Image.open(TRAINING_PATH+file), self.__labels_map__[int(key)]) )
        return out

    def get_files_tensor(self): 
        out = []
        for file in os.listdir(TRAINING_PATH):
            key = -1
            file_match = re.search(r"DATA_\d+_(\d+)\.png", file)
            key = file_match.group(1)
            if key == -1:
                raise ValueError(r"Match error: {file}")
            img = Image.open(TRAINING_PATH + file)
            img_tensor = self.transform(img)
            out.append( (img_tensor, self.__labels_map__[int(key)]) )
            #out.append( (decode_image(TRAINING_PATH + file), self.__labels_map__[int(key)]) ) 
        return out


    def plot(self):
        figure = plt.figure(figsize = (8,8))
        cols,rows = 11, 2
        for i in range(1, cols * rows):
            img, label = self.dataPIL[i]
            figure.add_subplot(rows, cols, i)
            plt.title(label)
            plt.axis("off")
            plt.imshow(img, cmap="gray")
        plt.show()


class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.flatten = nn.Flatten()
        self.linear_relu_stack = nn.Sequential(
                nn.Linear(64*64, 512),
                nn.ReLU(),
                nn.Linear(512, 512),
                nn.ReLU(),
                nn.Linear(512, 22),
        )

    def forward(self, x):
        #x = self.flatten(x)
        #logits = self.linear_relu_stack(x)
        #return logits
        x = x.view(x.size(0), -1)
        logits = self.linear_relu_stack(x)
        return logits


def piece_builder(piece, colour):
    out = 0b00000000
    
    if piece.lower() == 'pawn':
        out = 0b001 & 0b00000111
    elif piece.lower() == 'knight':
        out = 0b010 & 0b00000111
    elif piece.lower() == 'bishop':
        out = 0b011 & 0b00000111
    elif piece.lower() == 'rook':
        out = 0b100 & 0b00000111
    elif piece.lower() == 'queen':
        out = 0b101 & 0b00000111
    elif piece.lower() == 'king':
        out = 0b110 & 0b00000111
    elif piece.lower() == 'undefined':
        out = 0b111 & 0b00000000
    else:
        raise ValueError("Piece not specified")

    if colour.lower()[0] == 'b':
        out = out & 0b00000111
    elif colour.lower()[0] == 'w':
        out = out | 0b00001000
    else:
        raise ValueError("Colour not specified")
    
    return out


def take_screenshot(save: bool = False):
    global screenshot_count 
    image = pyautogui.screenshot()
    if save:
        image.save(f"{IMAGES_PATH}screenshot_{screenshot_count}.png")    
    screenshot_count += 1
    return image


def output_processed_array_as_img(squares):
    id_tag = int(input("Enter an id tag for this: "))
    keep_list = [0,1,2,3,4,5,6,7,10,11,17,18,49,52,56,57,58,59,60,61,62,63]
    keep_list_arr = range(0, len(keep_list))
    comp = {keep_list[i]:keep_list_arr[i] for i in range(len(keep_list))}
    for i in range(64):
        if i in keep_list:
            #cv2.imwrite(TRAINING_PATH + f"DATA_{screenshot_count}_{i}.png", squares[i])
            cv2.imwrite(TRAINING_PATH + f"DATA_{id_tag}_{comp[i]}.png", squares[i])

# Returns a 64-long array that breaks images 
def process(screenshot):
    image = np.array(screenshot)
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    _, thresh_image = cv2.threshold(gray_image, 127, 255, cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(thresh_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    board_contour = max(contours, key=cv2.contourArea)
    x, y, w, h = cv2.boundingRect(board_contour)
    chessboard = gray_image[y:y+h, x:x+w]

    if h != w:
        return None
    
    square_size = h//8
    squares = []
    
    for row in range(8):
        for col in range(8):
            square = chessboard[row * square_size:(row+1)*square_size, col*square_size:(col+1)*square_size]
            squares.append(square)
    
    return squares


def capture_training_data():
    global hashes
    #threading.Timer(10, main).start()
    current_state = process(take_screenshot(save=False)) 
    if current_state != None and current_state[0].size > 150: 
        byte_data = b''.join(ndarray.tobytes() for ndarray in current_state)
        hash_state = hashlib.sha256(byte_data).digest()
        if hash_state not in hashes:
            #print(f"New board detected - SHA256: {hash_state.hex()}")
            output_processed_array_as_img(current_state) 
            hashes.add(hash_state)
    else:
        print("No data found")


def train():
    device = (
        "cuda"
        if torch.cuda.is_available()
        else "mps"
        if torch.backends.mps.is_available()
        else "cpu"
    )
    print(f"Training on {device}")


    dataset = ChessboardSquares()
    #train_loader = DataLoader(dataset, batch_size = 40, shuffle = True)
    train_loader = DataLoader(dataset, batch_size = 22, shuffle = False)

    model = NeuralNetwork()
    optimizer = optim.Adam(model.parameters(), lr=0.001)
    criterion = nn.CrossEntropyLoss()

    # Training Loop
    epochs = 99 
    for epoch in range(epochs):
        model.train()
        running_loss = 0.0
        correct = 0
        total = 0

        for data, labels in train_loader:
            optimizer.zero_grad()  

            # Forward pass
            outputs = model(data)
            loss = criterion(outputs, labels)

            # Backward pass and optimize
            loss.backward()
            optimizer.step()

            # Calculate running statistics
            running_loss += loss.item()
            _, predicted = torch.max(outputs, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
        print(f"Epoch {epoch+1}/{epochs}, Loss: {running_loss/len(train_loader):.4f}, Accuracy: {100 * correct / total:.4f}%")



if __name__ == '__main__':
    #capture_training_data()
    train()
