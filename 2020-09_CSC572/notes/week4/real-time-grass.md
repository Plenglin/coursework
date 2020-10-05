# Grass

1. Grouping
    - Tiller: new stem on side of plant growing from root
    - Rhizome/Stolon: sent to create new grass formation
2. Visage
    - Coloring based on sunlight.
        - Dry grass turns gray or yellow because no water 
        - Low temp/high light is red to protect from intensive sunlight (or caused by disease)

# Methods for rendering

1. Pre-rendered
    - Less limited by performance
    - Polygonal mesh + 3D models
    - Movies
        - Incredibles (2004): noise fields, manually animated plants close to camera
        - Good Dinosaur (2015): Hierarchal curves, integration
    - Particles
2. Image-based
    - Render group on billboard/cross-tree 
        - Most frequent in computer games 
    - Orthmann method
        - Billboard as representation of group of grass blades
        - Multiple textures used
            - Greyscale defining coverage 
            - RGB growth direction
            - Greyscale with noise determining randomness
        - Streamed into pipeline and octree hierarchy created with billboards
    - Effective Collisions
        -  


