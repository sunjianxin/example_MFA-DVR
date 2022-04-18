## example_MFA-DVR
This repo provides examples of using MFA-DVR pipeline to visualize volumetric data, including encoding from raw data to MFA model and visualizing them using MFA-DVR.

###  Dependencies
- C++11 or higher compiler.
- [mfa](https://github.com/sunjianxin/mfa), Multivariate Functional Approximations (MFA) library.
- [VTK_MFA-DVR](https://github.com/sunjianxin/VTK_MFA-DVR), MFA-DVR libraries implemented on top of The Visualization Toolkit (VTK).
- [MPI](http://www.mpich.org)
   
### Build
1. Create project folder
```bash
mkdir myProject
cd myProject
```
2. Get mfa library, no need to build or install it.
```bash
git clone https://github.com/sunjianxin/mfa
```
3. Install VTK_DVR-MFA based on VTK of verion 9.0.3.
    * Follow the instruction in [VTK_DVR-MFA](https://github.com/sunjianxin/VTK_MFA-DVR) repo.
4. Get and build example code
```bash
git clone https://github.com/sunjianxin/example_MFA-DVR
cd example_MFA-DVR
mkdir build
cd build 
cmake ..  \
-DCMAKE_CXX_COMPILER=mpicxx \
-DMFA_INCLUDE_DIR=path_to_mfa_include_folder \
-DVTK_DIR:PATH=path_to_VTK_DVR-MFA_installation_folder
```
*path_to_mfa_include_folder* is the folder location in the project folder in step 2. *path_to_VTK_DVR-MFA_installation_folder* is the installation location when you configure VTK_DVR-MFA before building, and it is normally at */usr/local/include/vtk-9.0* by default.
### Run
1. Encoding raw volumetric data into MFA model
2. Visualize 
