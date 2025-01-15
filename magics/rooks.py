import random

# Initialize blockers set
blocker_indices = [1, 2, 3, 4, 5, 6, 8, 16, 24, 32, 40, 48]
blockers_array = []

# Generate all blockers combinations
for i in range(pow(2, 12)):
    blockers = 0b0  
    number = bin(i)[2:].zfill(12)
    number = number[::-1]

    for index, bit in enumerate(number):
        if int(bit) & 1:
            blockers |= (1 << blocker_indices[index])

    blockers_array.append(blockers)

# Function to generate a random 64-bit unsigned integer
def generate_random_64bit():
    return random.randint(0, (1 << 64) - 1)

# Multiply the random number by each blocker, shift right by 52, and mask to get the lower 12 bits
while True:
    # Set to track unique results of multiplication
    results_array = []

    # Generate a random 64-bit unsigned integer
    magic = generate_random_64bit()
    for blocker in blockers_array:
        result = (magic * blocker) & 0xFFFFFFFFFFFFFFFF 
        shifted_result = result >> 51  # Right shift the result by 52 bits
        if shifted_result in results_array:
            break
        else:
            results_array.append(shifted_result)

    if (len(results_array) == pow(2, 12)):
        print(f'magic: {magic}')
        results_array = sorted(results_array)
        for num in results_array:
            print(results_array)
        break
