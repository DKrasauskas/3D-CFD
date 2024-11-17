# *Lattice Boltzman 3D CFD*
![image](https://github.com/user-attachments/assets/12eb5bb7-c60b-4c26-9c23-67121bcc17d5)


LBM CFD D3Q19 lattice using Bhatnagar Gross and Krook (BGK) relaxation method. Current setup contains 60mil voxels, as limited per my GPU memory. The visualization is done by computing the local curl at each point, and rendered using Marching Cubes algorithm. 


## A220-300 
https://github.com/user-attachments/assets/348d50da-62d7-4c84-89e4-b1ec237c163e

## Aerodynamics of a Cow
https://github.com/user-attachments/assets/9145dea6-e826-4f80-9725-6f81bc80a3a9





### Settings
In settings.h simulation parameters and controls can be tweaked as per users choice. The launch parameters, including the grid size, scale, as well as the LBM parameters such as viscosity, Reynolds number and free stream velocity can be tweaked as per users choice. In controls.h, control settings such as movement of the camera can be tweaked as per users choice. 
