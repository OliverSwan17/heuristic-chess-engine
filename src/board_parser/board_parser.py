#!/usr/bin/env python3
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
                               0: 'lbr',    # Light Black Rook
                               1: 'dbk',    # Dark  Black Knight
                               2: 'lbb',    # Light Black Bishop
                               3: 'dbq',    # Dark  Black Queen
                               4: 'lbK',    # Light Black King
                               5: 'dbb',    # Dark  Black Bishop
                               6: 'lbk',    # Light Black Knight
                               7: 'dbr',    # Dark  Black Rook
                               8: 'dbp',    # Dark  Black Pawn
                               9: 'lbp',    # Light Black Pawn
                               10: 'due',   # Dark  Undef Empty
                               11: 'lue',   # Light Undef Empty
                               12: 'dwp',   # Dark  White Pawn
                               13: 'lwp',   # Light White Pawn
                               14: 'dwr',   # Dark  White Rook
                               15: 'lwk',   # Light White Knight
                               16: 'dwb',   # Dark  White Bishop
                               17: 'lwq',   # Light White Queen
                               18: 'dwK',   # Dark  White King
                               19: 'lwb',   # Light White Bishop
                               20: 'dwk',   # Dark  White Knight
                               21: 'lwr',   # Light White Rook
                               22: 'dbK',   # Dark  Black King
                               23: 'lwK',   # Light White King
                               24: 'lbq',   # Light Black Queen
                               25: 'dwq'}   # Dark  White Queen

        self.label_to_index = {label: idx for idx, label in self.__labels_map__.items()}
        self.transform = transforms.Compose([transforms.Resize(size=(64, 64)), transforms.ToTensor()])
        self.dataPIL = self.get_files_PIL()
        self.dataTensor = self.get_files_tensor()
        self.len = self.__len__()

    def __len__(self):
        return len(self.dataPIL)

    def __getitem__(self, idx):
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

"""
class NeuralNetwork(nn.Module):
    def __init__(self):
        super(NeuralNetwork, self).__init__()

        # First convolutional block - change the input channels to 1 for grayscale
        self.conv1 = nn.Conv2d(1, 32, kernel_size=3, padding=1)  # 1 input channel for grayscale
        self.bn1 = nn.BatchNorm2d(32)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.bn2 = nn.BatchNorm2d(64)
        self.pool = nn.MaxPool2d(2, 2)

        # Second convolutional block
        self.conv3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.bn3 = nn.BatchNorm2d(128)

        # Fully connected layers
        self.fc1 = nn.Linear(128 * 8 * 8, 512)  # Adjust the size based on image dimensions
        self.fc2 = nn.Linear(512, 26)  # Number of classes for piece type and color

        # Dropout layer to prevent overfitting
        self.dropout = nn.Dropout(0.5)

    def forward(self, x):
        x = torch.relu(self.bn1(self.conv1(x)))
        x = self.pool(x)
        x = torch.relu(self.bn2(self.conv2(x)))
        x = self.pool(x)
        x = torch.relu(self.bn3(self.conv3(x)))
        x = self.pool(x)

        # Flatten the output for fully connected layers
        x = x.view(-1, 128 * 8 * 8)
        x = torch.relu(self.fc1(x))
        x = self.dropout(x)  # Apply dropout during training
        x = self.fc2(x)
        return x
"""

class NeuralNetwork(nn.Module):
    def __init__(self):
        super(NeuralNetwork, self).__init__()

        # First convolutional block
        self.conv1 = nn.Conv2d(1, 64, kernel_size=5, padding=2)  # Increased number of filters and kernel size
        self.bn1 = nn.BatchNorm2d(64)
        self.conv2 = nn.Conv2d(64, 128, kernel_size=3, padding=1)  # Increased filters
        self.bn2 = nn.BatchNorm2d(128)
        self.pool = nn.MaxPool2d(2, 2)

        # Second convolutional block
        self.conv3 = nn.Conv2d(128, 256, kernel_size=3, padding=1)  # Increased filters
        self.bn3 = nn.BatchNorm2d(256)
        
        # Third convolutional block (optional)
        self.conv4 = nn.Conv2d(256, 512, kernel_size=3, padding=1)  # Adding more layers
        self.bn4 = nn.BatchNorm2d(512)

        # Fully connected layers
        self.fc1 = nn.Linear(512 * 4 * 4, 1024)  # Increased number of neurons
        self.fc2 = nn.Linear(1024, 512)  # Adding another fully connected layer
        self.fc3 = nn.Linear(512, 26)  # Output layer for 26 classes (pieces and squares)

        # Dropout layer to prevent overfitting
        self.dropout = nn.Dropout(0.5)

    def forward(self, x):
        # Convolutional Layers with ReLU and Pooling
        x = torch.relu(self.bn1(self.conv1(x)))
        x = self.pool(x)
        x = torch.relu(self.bn2(self.conv2(x)))
        x = self.pool(x)
        x = torch.relu(self.bn3(self.conv3(x)))
        x = self.pool(x)
        x = torch.relu(self.bn4(self.conv4(x)))  # Add the fourth convolutional block
        x = self.pool(x)

        # Flatten the output for fully connected layers
        x = x.view(-1, 512 * 4 * 4)  # Adjust this based on input size and layer depth
        x = torch.relu(self.fc1(x))
        x = self.dropout(x)  # Apply dropout during training
        x = torch.relu(self.fc2(x))  # Additional fully connected layer
        x = self.fc3(x)  # Final output layer
        return x




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

# This looks at squares 16, 17, 24, 25
def output_processed_array_as_img_special(squares, tag):
    keep_list = [16, 17, 24, 25] 
    keep_list_arr = {17:22, 25:23, 16:24, 24:25}
    comp = keep_list_arr

    for i in range(64): 
        if i in keep_list:
            cv2.imwrite(TRAINING_PATH + f"DATA_{tag}_{comp[i]}.png", squares[i])


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



def capture_training_data_special(tag):
    current_state = process(take_screenshot(save=False)) 
    if current_state != None and current_state[0].size > 150: 
        output_processed_array_as_img_special(current_state, tag) 
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
    train_loader = DataLoader(dataset, batch_size = 64, shuffle = True)

    model = NeuralNetwork() # TOY WITH THESE VALUES
    optimizer = optim.Adam(model.parameters(), lr=0.0005, weight_decay = 1e-5)
    criterion = nn.CrossEntropyLoss()

    best_acc = -1
    best_loss = -1

    # Training Loop
    epochs = 100 
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
       
        loss = running_loss/len(train_loader)
        acc = correct/total

        #print(f"Epoch {epoch+1}/{epochs}, Loss: {running_loss/len(train_loader):.4f}, Accuracy: {100 * correct / total:.4f}%")
        print(f"Epoch {epoch+1}/{epochs}, Loss: {(loss):.4f}, Accuracy: {acc*100:.4f}%")
        
        if (best_acc == -1) and (best_loss == -1):
            print("Tentative model...")
            best_acc = acc
            best_loss = loss
            torch.save(model.state_dict(), DIR_PATH + "/" + "model.pth")

        if acc >= best_acc and loss <= best_loss:
            print("Saving model...")
            best_acc = acc
            best_loss = loss
            torch.save(model.state_dict(), DIR_PATH + "/" + "model.pth")

    print(f"\nBest model saved. Accuracy: {best_acc}\tLoss: {best_loss}")

def piece_builder(piece, colour):
    out = 0b00000000
    
    if piece.lower() == 'p':
        out = 0b001 & 0b00000111
    elif piece.lower() == 'k':
        out = 0b010 & 0b00000111
    elif piece.lower() == 'b':
        out = 0b011 & 0b00000111
    elif piece.lower() == 'r':
        out = 0b100 & 0b00000111
    elif piece.lower() == 'q':
        out = 0b101 & 0b00000111
    elif piece.lower() == 'K':
        out = 0b110 & 0b00000111
    elif piece.lower() == 'u':
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

def process2(screenshot):
    image = np.array(screenshot)
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    _, thresh_image = cv2.threshold(gray_image, 127, 255, cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(thresh_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    board_contour = max(contours, key=cv2.contourArea)
    x, y, w, h = cv2.boundingRect(board_contour)
    chessboard = gray_image[y:y+h, x:x+w]

    if h != w:
        return None
    
    square_size = h // 8
    squares = []
    
    for row in range(8):
        for col in range(8):
            square = chessboard[row * square_size:(row + 1) * square_size, col * square_size:(col + 1) * square_size]
            
            # Resize each square to 64x64 and convert to tensor
            square_resized = cv2.resize(square, (64, 64))  # Resize to match model input
            square_resized = np.expand_dims(square_resized, axis=0)  # Add the channel dimension (1 for grayscale)
            square_tensor = torch.tensor(square_resized, dtype=torch.float32)  # Convert to tensor
            
            # Normalize the tensor to be between 0 and 1
            square_tensor = square_tensor / 255.0
            squares.append(square_tensor)
    
    return squares


def translate():


    labels_map = {
                               0: 'lbr',    # Light Black Rook
                               1: 'dbk',    # Dark  Black Knight
                               2: 'lbb',    # Light Black Bishop
                               3: 'dbq',    # Dark  Black Queen
                               4: 'lbK',    # Light Black King
                               5: 'dbb',    # Dark  Black Bishop
                               6: 'lbk',    # Light Black Knight
                               7: 'dbr',    # Dark  Black Rook
                               8: 'dbp',    # Dark  Black Pawn
                               9: 'lbp',    # Light Black Pawn
                               10: 'due',   # Dark  Undef Empty
                               11: 'lue',   # Light Undef Empty
                               12: 'dwp',   # Dark  White Pawn
                               13: 'lwp',   # Light White Pawn
                               14: 'dwr',   # Dark  White Rook
                               15: 'lwk',   # Light White Knight
                               16: 'dwb',   # Dark  White Bishop
                               17: 'lwq',   # Light White Queen
                               18: 'dwK',   # Dark  White King
                               19: 'lwb',   # Light White Bishop
                               20: 'dwk',   # Dark  White Knight
                               21: 'lwr',   # Light White Rook
                               22: 'dbK',   # Dark  Black King
                               23: 'lwK',   # Light White King
                               24: 'lbq',   # Light Black Queen
                               25: 'dwq'}   # Dark  White Queen

    model = NeuralNetwork()
    model.load_state_dict(torch.load(DIR_PATH + "/" + "model.pth"))
    model.eval()

    input("Press any input to continue. Make sure you have a chess game open on your primary window: ")

    # Process the screenshot to get individual squares
    data = process(take_screenshot(save=False))

    if data is not None and len(data) == 64:  # Ensure we have 64 squares to process
        for idx, square in enumerate(data):
            square_tensor = torch.tensor(square, dtype=torch.float32)
            resize_transform = transforms.Compose([
                transforms.ToPILImage(),  # Convert numpy array to PIL image
                transforms.Resize((64, 64))  # Resize to 64x64
            ])
            square_tensor = resize_transform(square_tensor)
            square_tensor = transforms.ToTensor()(square_tensor).unsqueeze(0)  # Add batch dimension
            square_tensor = square_tensor.squeeze(0)  # Remove the batch dimension
            square_tensor = square_tensor.unsqueeze(0)  # Add the batch dimension back
            with torch.no_grad():
                output = model(square_tensor)  # Feed the individual square through the model
            predicted_label = torch.argmax(output, dim=1)  # Get the predicted label
            print(f"Square {idx} predicted label: {labels_map[predicted_label.item()]}")  # Print the prediction for each square
    else:
        print("Error: No valid chessboard found.")

if __name__ == '__main__':
    #train()
    translate()


