# *Lattice Boltzman 3D CFD*
![image](https://github.com/user-attachments/assets/12eb5bb7-c60b-4c26-9c23-67121bcc17d5)


Barnes-Hut gravity simulation utilizing a GPU based Quad-Tree. Written using Cuda and Thrust.
Tree construction on GPU was implemented based on the paper by J. Zhang and L. Gruenwald (https://adms-conf.org/2019-camera-ready/zhang_adms19.pdf)  with some minor modifications.
The bottleneck of the simulation is the acceleration computation for the particles, as the tree construction step tends to be really fast. 


## Simulation with quadtree visualization 1mil bodies
https://github.com/user-attachments/assets/6690395c-04b0-4d9b-9326-5763cbf6f1f4

## Regular simulation 1mil bodies
https://github.com/user-attachments/assets/1524a8f6-b0e0-4c49-ae47-a811b202b221

The simulation can be run in real time with 1mil+ bodies, but accuracy must be reduced by changing the phi parameter.

### Settings
In settings.h simulation parameters and controls can be tweaked as per users choice. The number of particles are entered into the console by the user, even though no explicit limit on the number of particles exists. Therefore, entering too many particles may cause some errors (for now, up to 4mil particles have been tested)
