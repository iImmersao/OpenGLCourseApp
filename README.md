# OpenGLCourseApp
Repo for learning OpenGL in C++.

This application requires the Assimp library for loading models. This needs to be built from source, which can be cloned with:

git clone https://github.com/assimp/assimp.git assimp

You will also require CMake, in order to set-up the Assimp code for building in Visual Studio. Use the .msi installer and choose the option to update the PATH variable, so that it is accessible from the command line.

Also ensure that the MSBuild.exe within your Visual Studio installation is accessible via the PATH.

In order to configure Assimp for building, execute CMake in the top-level directory of the cloned source. CMake needs to be given some parameters, in order to prepare the project for building in the specific version of Visual Studio that you are using. For example, for VS 2022:

> cmake-G "Visual Studio 17 2022" -A x64 ./

This also prepares Assimp for building on the amd64 architecture.

After running CMake, a .sln file will be present, which can then be opened in Visual Studio. Within Visual Studio, select the appropriate type of build, such as "Release" and "x64". Then go to the "Build" menu and select "Build Solution". This should run through successfully and create some libraries, which you will need to make available in your OpenGLCourseApp Visual Studio project in much the same way as with the OpenGL libraries, along with the header files for the library.

There appears to be a bug in the Assimp build. One of the commonly-used include files references another include file, which is not accessible through the default set-up. Wherever you set up the "assimp" includes directory, also copy in the "contrib" includes directory, as headers within the assimp directory refer to files in contrib.
