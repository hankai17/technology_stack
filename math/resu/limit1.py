#!/usr/bin/python3
#coding=utf8

import matplotlib.pyplot as plt
import numpy as np
import sympy as sp

def test1():
    x = np.arange(-10, 10, 0.01)
    y = x * np.sin(1 / x)
    plt.figure()
    plt.plot(x, y)
    plt.grid(True)
    plt.show()

def test1_1():
    x = sp.symbols('x') 
    y = x * sp.sin(1 / x)
    print(sp.limit(y, x, 0))

def test2():
    x = np.arange(0, 10, 0.01)
    y = np.log(x) / (x ** 2)
    plt.figure()
    plt.plot(x, y)
    plt.grid(True)
    plt.show()

def test2_2():
    x = sp.symbols('x')
    y = sp.log(x) / (x ** 2)
    print(sp.limit(y, x, 0, dir="+"))


#test1()
#test1_1()
#test2()
#test2_2()
