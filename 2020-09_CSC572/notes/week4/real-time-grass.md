1. Grass
    1. Grouping 
        - Tiller: new stem on side of plant growing from root
        - Rhizome/Stolon: sent to create new grass formation
    2. Visage
        - Coloring based on sunlight.
            - Dry grass turns gray or yellow because no water 
            - Low temp/high light is red to protect from intensive sunlight (or caused by disease)
1. Methods for rendering
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
        - Collision phases
            1. Runtime refinement
            2. Collision detection
            3. Collision reaction
            4. Recovering
            5. Simplification
    - Banisch-Wuthrich
        - Generate a multiple shell layers, each with individual elements
        - Use mass-spring to make shells move together
        - Use prisms for clusters and connect with neighbors
    - Fan algorithm
    - Jahrmann-Wimmer 2017
        - 3 control points
            - Bottom $v_0$ fixed position of blade
            - Top $v_2$ moved
            - $v_1$ computed based on $v_0, v_2$ as foundation for curvature
        - Other parameters: height, width, stiffness, up-vec, rotation
        - Distribution with Poisson-disk
        - $\sigma = (r + g + w)\Delta t + d$ where $r, g, w, d$ are recovery, gravity, wind, collision displacement
        - Sphere collisions for quick maths
        - Methods used to limit translation/displacement to prevent deformation
3. Parallel Poisson Disk Sampling
    - Uniform random causes overlaps and artifacts. Poisson prevents it
4. Vigor
    - XBox game, third-person shooter, Unreal Engine, with lots of Norwegian countryside
    - Requirements
        - $80000m^2$ of grass visible
5. Implementation 
    - Each blade rendered as an instance
    - LERP for shadow calculations

