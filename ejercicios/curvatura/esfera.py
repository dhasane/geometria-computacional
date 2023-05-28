import math
import random as r
import matplotlib.pyplot as plt
import numpy as np
from numpy import linalg as LA

def get_punto(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        radio * math.cos(rad_1) * math.sin(rad_2),
        radio * math.sin(rad_1) * math.sin(rad_2),
        radio * math.cos(rad_2)
    ])

def derivada_parcial_u(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        -radio * math.sin(rad_1) * math.sin(rad_2),
        radio * math.cos(rad_1) * math.sin(rad_2),
        0
    ])

def derivada_parcial_uu(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        -radio * math.cos(rad_1) * math.sin(rad_2),
        -radio * math.sin(rad_1) * math.sin(rad_2),
        0
    ])

def derivada_parcial_v(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        radio * math.cos(rad_1) * math.cos(rad_2),
        radio * math.sin(rad_1) * math.cos(rad_2),
        -radio * math.sin(rad_2)
    ])

def derivada_parcial_vv(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        -radio * math.cos(rad_1) * math.sin(rad_2),
        -radio * math.sin(rad_1) * math.sin(rad_2),
        -radio * math.cos(rad_2)
    ])

def derivada_parcial_uv(radio, ang_1, ang_2):
    radio = 2 * math.pi
    rad_1 = ang_1 * radio
    rad_2 = ang_2 * radio
    return np.array([
        -radio * math.sin(rad_1) * math.cos(rad_2),
        radio * math.cos(rad_1) * math.cos(rad_2),
        0
    ])

def norma(rr):
    return math.sqrt(
        rr ** 2
    )

def nuv(r, u, v):
    rr = np.dot(derivada_parcial_u(r,u,v), derivada_parcial_v(r,u,v))
    nrma = LA.norm(rr)
    return rr / nrma

def I1(r, u, v):
    return {
        'E' : np.dot(derivada_parcial_u(r,u,v), derivada_parcial_u(r,u,v)),
        'F' : np.dot(derivada_parcial_u(r,u,v), derivada_parcial_v(r,u,v)),
        'G' : np.dot(derivada_parcial_v(r,u,v), derivada_parcial_v(r,u,v))
    }

def I2(r, u, v):
    return {
        'L' : np.dot(nuv(r,u,v), derivada_parcial_uu(r,u,v)),
        'M' : -np.dot(nuv(r,u,v), derivada_parcial_uv(r,u,v)),
        'N' : np.dot(nuv(r,u,v), derivada_parcial_vv(r,u,v))
    }

def k(a,b,c):
    aa = math.sqrt(b**2 - 4*a*c)

    ret = (
        (-b + aa) / 2 * a,
        (-b - aa) / 2 * a
    )


def curvaturas_principales():
    i1 = I1
    i2 = I2

def get_n_points(radio, n):
    pts = []
    for _ in range(n):
        rand_1 = r.random()
        rand_2 = r.random()

        # print(I1(r, rand_1, rand_2))
        # print(I2(r, rand_1, rand_2))

        pts += [get_punto(radio, rand_1, rand_2)]

    return pts

def draw(radio, n):

    ax = plt.figure().add_subplot(projection='3d')

    x = []
    y = []
    z = []

    for (x_p, y_p, z_p) in get_n_points(radio, n):
        x += [x_p]
        y += [y_p]
        z += [z_p]

    print(x)
    print(y)
    print(z)
    ax.scatter(x, y, z, zdir='y', label='points in (x, z)')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    # ax.view_init(elev=20., azim=-35, roll=0)

    plt.show()

# get_n_points(5, 10)

draw(5, 1)
