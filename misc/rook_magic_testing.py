import random

blocker_indices = [1, 2, 3, 4, 5, 6, 8, 16, 24, 32, 40, 48]
blockers_array = []

for i in range(pow(2, 12)):
    blockers = 0b0  
    number = bin(i)[2:].zfill(12)
    number = number[::-1]

    for index, bit in enumerate(number):
        if int(bit) & 1:
            blockers |= (1 << blocker_indices[index])

    blockers_array.append(blockers)

def generate_random_64bit():
    return random.randint(0, (1 << 64) - 1)

while True:
    results_array = []

    magic = generate_random_64bit()
    for blocker in blockers_array:
        result = (magic * blocker) & 0xFFFFFFFFFFFFFFFF 
        shifted_result = result >> 51
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
