#!/usr/bin/python3
#coding=utf8

import matplotlib.pyplot as plt
import numpy as np
import sympy as sp

def test0():
    x = sp.symbols('x')
    print(sp.limit(1 / (x + 1) - 2 / (x ** 3 - 2), x, 1))

def test1():
    x, n = sp.symbols('x, n')
    print(sp.limit(((3 * x - 2) / (x + 3)) ** n, x, sp.oo))

def sinx_x():
    x = np.arange(-5 * np.pi, 5 * np.pi, 0.01)
    y = np.sin(x) / x
    plt.figure()
    plt.plot(x, y)
    plt.grid(True)
    plt.show()

def limit_sinx_x():
    x = sp.symbols('x')
    y = sp.sin(x) / x
    print(sp.limit(y, x, 0))

def x():
    x1 = np.arange(-100, -2, 0.01)
    x2 = np.arange(0, 100, 0.01)
    y1 = (1 + 1 / x1) ** x1
    y2 = (1 + 1 / x2) ** x2
    plt.figure()
    plt.plot(x1, y1, x2, y2)
    plt.grid(True)
    plt.show()

def limit_x():
    x = sp.symbols('x')
    y = (1 + 1 / x) ** x
    print(sp.limit(y, x, sp.oo))
    print(sp.limit(y, x, -sp.oo))
    print(sp.limit(y, x, 0, dir="+"))
    print(sp.limit(y, x, 0, dir="-"))

#sinx_x()
limit_x()
x()
