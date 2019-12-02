# -*- coding: utf-8 -*-
"""
Created on Wed Feb 17 00:37:04 2016

@author: fh

An arithmetic calculator for non-negative integers represented in arbitrary base
"""


class BaseNum:
    '''
    Class that holds a number which is represented internally as a sequence of integers.
    Its methods include initialization with int\string\list, copying, simplifying to legal representation, and convert to a string. 
    The digit in the number can be indexed and iterate over.    
    '''
    base = 100  # default base = 100
    bl = 2      # base length
    
    def __init__(self, st):
        if type(st) is str:
            # split string into 2-character sequence in reverse order      
            self.num = [int(st[max(i,0):i+self.bl]) for i in range(len(st)-self.bl, -self.bl, -self.bl)]
            self.simplify()  # derive a legal representation
        elif type(st) is list:
            # save the list directly, 
            # don't simplify it since this is used mostly as an space allocating procedure
            self.num = st.copy()
        elif type(st) is int:
            self.num = [st]
            self.simplify()  # derive a legal representation
            
    def copy(self):
        return BaseNum(self.num)
    
    def simplify(self):
        'derive a legal representation'
        if self.num == []: return self  # does not simplify empty object
        # legal integer range
        i = 0
        while i<len(self):
          if not (0 <= self[i] < self.base):
              self[i+1] += self[i] // self.base
          self[i] %= self.base
          i += 1
        # delete prepending zeros
        i = len(self)
        while i>0 and self.num[i-1] == 0: 
            i -= 1
        self.num = self.num[:i] if i>0 else [0]
        return self
            
    def __str__(self):
        # representation in string format, delete 0s at the front
        t = ''.join([str(t).zfill(BaseNum.bl) for t in reversed(self.copy().simplify())]).lstrip('0')
        return '0' if t=='' else  t
        
    def __len__(self):
        return len(self.num)
        
    def __getitem__(self, key):      # allow indexing that exceeds length
        try:
            return self.num[key]
        except IndexError:
            return 0
    
    def __setitem__(self, key, value):
        try:
            self.num[key] = value
        except IndexError:
            if key == len(self.num):  # allow extension of length by one
                self.num += [value]
            else:
                raise IndexError
                
    def __iter__(self):
        return self.num
        
    def __reversed__(self):
        return reversed(self.num)

        
# Following functions operate on BaseNum instances
def isGreaterThan(a, b):
    'use legal representation of a & b, otherwise it may give incorrect results'
    if len(a) > len(b): 
        return 1
    elif len(a) < len(b):
        return -1
    else:
        for ai, bi in zip(reversed(a), reversed(b)):
            if ai > bi:
                return 1
            elif ai < bi:
                return -1
        return 0
        

def isSmallerThan(a, b):
    return - isGreaterThan(a, b)
        

def add(a, b):
    # utilize BaseNum properties of indexing
    l = max(len(a), len(b))
    t = BaseNum('0' * (l*BaseNum.bl))
    for i in range(l):
        t[i] += (a[i] + b[i]) 
        t[i+1] += t[i] // BaseNum.base 
        t[i] %= BaseNum.base
    return t
    
    
def subtract(a, b):
    if isGreaterThan(a, b) == -1:  # a<b
        return BaseNum(0)
    else:
        # similar to add
        l = max(len(a), len(b))
        t = BaseNum([0] * l)
        for i in range(l):
            t[i] += (a[i] - b[i])
            t[i+1] += t[i] // BaseNum.base
            t[i] %= BaseNum.base
        t.simplify()   # very important
        return t       
    

def multiply(a, b):
    t = BaseNum([0] * (len(a)+len(b)))
    for i in range(len(a)):
        for j in range(len(b)):
            t[i+j] += a[i] * b[j]
            t[i+j+1] += t[i+j] // BaseNum.base
            t[i+j] %= BaseNum.base
    t.simplify()  # very important
    return t


def concat(a, b):
    t = BaseNum([])
    t.num = a.num + b.num  # concat two lists
    return t
    
    
def quotient(a, b): 
    'use legal representation of a, b, otherwise algorithm can fall'
    t = BaseNum([0]*len(a))
    while isGreaterThan(a, b)>=0:
        # first slice a to get the part larger than b, which is stored in a1
        # store the reminder part in a2
        if isGreaterThan(BaseNum(a[-len(b):]), b) >= 0:
            a1, a2 = BaseNum(a[-len(b):]), BaseNum(a[:-len(b)])
        else:
            a1, a2 = BaseNum(a[-len(b)-1:]), BaseNum(a[:-len(b)-1])
        # then get the quotient starting with a guess
        if a1[-1] >= b[-1]:
            guess = (a1[-1] // b[-1])
        else:
            guess = (a1[-1]*BaseNum.base + a1[-2]) // b[-1]
        # increase guess until too large
        while isGreaterThan(a1, multiply(BaseNum(guess), b))>=0: guess+=1
        # decrease guess to a proper value
        while isGreaterThan(a1, multiply(BaseNum(guess), b))<0 : guess-=1
        
        t[len(a2)] = guess
        r = subtract(a1, multiply(BaseNum(guess), b))
        a = concat(a2, r).simplify()   # must simplify to get legal a & b
    t.simplify()
    return t      
            

def remainder(a, b):
    return subtract(a, multiply(b, quotient(a, b)))
    
    
def quotientAndRemainder(a, b):
    return quotient(a, b), remainder(a, b)


def sharp(a, b, m):
    return BaseNum(multiply(a, b)[m:])
    

# --------------Test Code for base = 10----------------
import random
BaseNum.base = 10
BaseNum.bl = 1
for i in range(10): # Test 100 times
    a = random.randint(0, 10000000)
    b = random.randint(1, 1000)
    m = 4
    if str(a+b) != str(add(BaseNum(a), BaseNum(b))):
        print("a={0}, b={1}, a+b={2}, but get {3}".format(a, b, a+b, add(BaseNum(a), BaseNum(b))))
    if str(max(a-b,0)) != str(subtract(BaseNum(a), BaseNum(b))):
        print("a={0}, b={1}, a-b={2}, but get {3}".format(a, b, a-b, subtract(BaseNum(a), BaseNum(b))))
    if str(a*b) != str(multiply(BaseNum(a), BaseNum(b))):
        print("a={0}, b={1}, a*b={2}, but get {3}".format(a, b, a*b, multiply(BaseNum(a), BaseNum(b))))
    if str(a//b) != str(quotient(BaseNum(a), BaseNum(b))):
        print("a={0}, b={1}, a//b={2}, but get {3}".format(a, b, a//b, quotient(BaseNum(a), BaseNum(b))))
    if str(a%b) != str(remainder(BaseNum(a), BaseNum(b))):
        print("a={0}, b={1}, a%b={2}, but get {3}".format(a, b, a%b, remainder(BaseNum(a), BaseNum(b))))
    if str(a*b//10**m) != str(sharp(BaseNum(a), BaseNum(b), m)):
        print("a={0}, b={1}, a#b={2}, but get {3}".format(a, b, a*b//10^m, sharp(BaseNum(a), BaseNum(b), m)))


# -------------- Test codes for input file, with base = 100------
BaseNum.base = 100
BaseNum.bl = 2

infilename = 'fin.txt'
outfilename = 'fout.txt'

ops = [isSmallerThan, add, subtract, multiply, quotientAndRemainder, sharp]
sops = ['<','+','-','*','/','#']

with open(infilename, 'r') as fin, open(outfilename,'w') as fout:
    fin.readline()   # ignore first line
    for line in fin:
        n, a, b, *_ = line.split()
        n = sops.index(n)
        a = BaseNum(a)
        b = BaseNum(b)
        if n < 5:
            result = ops[n](a,b)
        else:
            m = int(_[0])
            result = ops[n](a,b,m)
        if n!=4:
            fout.write(str(result)+'\n')
        else:
            fout.write(str(result[0])+' ' +str(result[1])+'\n')

