import random
import matplotlib.pyplot as plt
import string

collisions = 0
indices = set()
times = 0

class Node:
    def __init__(self, key):
        self.key = key
        self.value = self.generate_random_string()

    def generate_random_string(self,length=16):
        characters = string.ascii_letters + string.digits
        random_string = ''.join(random.choice(characters) for _ in range(length))
        return random_string

class HashTable:
    def __init__(self, size):
        self.size = size
        self.table = [None] * size 
    
    def hash(self, x):
        global collisions
        global indices
        global times
        times += 1
        x = (x + 0x9E3779B97F4A7C15) & 0xFFFFFFFFFFFFFFFF
        x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9 & 0xFFFFFFFFFFFFFFFF
        x = (x ^ (x >> 27)) * 0x94D049BB133111EB & 0xFFFFFFFFFFFFFFFF
        result = x ^ (x >> 31)
        result = result % self.size # 4
        if result in indices:
            collisions += 1
        
        indices.add(result)
        return result


if __name__ == "__main__":
    dupps = set()
    n = 1000
    hF = HashTable(n)
    
    u=[]
    v=[]
    
    average = 0
    for k in range (20):
        for i in range (n):
            u.append(i)
            x = random.randint(-(2**31),(2**31)-1)
            if(x not in dupps):
                dupps.add(x)
                hF.hash(x)
                v.append(times)
                times = 0
        print("Collisions: "+ str(collisions))
        dupps = set()
        indices = set()
        average += collisions
        collisions = 0

    print("Average collisions: " + str(average / 20))