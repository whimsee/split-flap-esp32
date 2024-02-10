import math

# Pitch Radius:
# flaps * (hole * 2 + spaces) / ( 2 * PI );

# Outer diameter
# Pitch Radius + flap_hole_radius + outset

def pitch_radius(flaps, hole_radius, separation):
    return flaps * (hole_radius * 2 + separation) / ( 2 * math.pi)

def outer_diameter(p_radius, hole_radius):
    return p_radius + (hole_radius * 2)

def get_separation(flaps, hole_radius, spool_radius=19):
    x = flaps * (hole_radius * 2)
    y = (spool_radius - hole_radius * 2) * (2 * math.pi)
    z = (y - x) / flaps
    return z
#     (flaps * (hole_radius * 2)) + (flaps * separation) = spool_radius - hole_radius / (2 * math.pi)

flaps = 60
hole_radius = .5

print(get_separation(flaps, hole_radius))

t = pitch_radius(flaps, hole_radius, get_separation(flaps, hole_radius))

print(outer_diameter(t, hole_radius))