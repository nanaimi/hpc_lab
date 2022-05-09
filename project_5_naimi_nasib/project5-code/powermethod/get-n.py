 #!//usr/bin/python3
# p, N
import sys
import math

p =  int(sys.argv[1])
N = int(sys.argv[2])

print(p * int(N * math.sqrt(p)/p))