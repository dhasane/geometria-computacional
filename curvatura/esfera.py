import math
import random as r
import matplotlib.pyplot as plt

def get_punto(radio, ang_1, ang_2):
    tpi = 2 * math.pi
    rad_1 = ang_1 * tpi
    rad_2 = ang_2 * tpi
    return (
        radio * math.cos(rad_1) * math.sin(rad_2),
        radio * math.sin(rad_1) * math.sin(rad_2),
        radio * math.cos(rad_2)
    )

def get_n_points(radio, n):
    pts = []
    for _ in range(n):
        rand_1 = r.random()
        rand_2 = r.random()
        pts += [get_punto(radio, rand_1, rand_2)]

    return pts

def draw(radio):

    ax = plt.figure().add_subplot(projection='3d')

    x = []
    y = []
    z = []

    for (x_p, y_p, z_p) in get_n_points(radio, 1000):
        x += [x_p]
        y += [y_p]
        z += [z_p]
    ax.scatter(x, y, z, zdir='y', label='points in (x, z)')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    # ax.view_init(elev=20., azim=-35, roll=0)

    plt.show()

# get_n_points(5, 10)

draw(5)
