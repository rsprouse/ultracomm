ultracomm
=========

A command line utility for communicating with and acquiring data from an Ultrasonix ultrasound machine.


== Prerequisites:


 - Visual Studio 2010 Express Edition from Microsoft, Service Pack 1 or greater.

 - boost (boost.org) libraries, especially program options library.

 - CMake from KitWare.

 - Ultrasonix SDK to match your version of Exam.


# TODO: have cmake autogenerate a header file with sdk version number and make available from ultracomm
# (Partially complete, see CMakeLists.txt in top level.)

== Compiling

 - Edit the CMakeLists.txt file in the root directory of the ultracomm project.
   Find the lines where ULTRASONIX_SDK_VERSION, ULTRASONIX_SDK_PATH, and ULTERIUS_LIB_NAME
   are defined and set them to match your version of the Ultrasonix SDK and its location.

1. Create a build folder wherever you like on your system.

1. Run the CMake gui.

1. Provide the absolute path to the project root in the "Where is the source code:" 
   box, e.g. C:\src\ultracomm.

1. Provide the absolute path to your build folder in the "Where to build the
   binaries:" box, e.g. C:\build\ultracomm.

1. Press the Configure button. Select the "Visual Studio 10" as the
   "generator for this project".

1. Press Generate. 

1. Look for the file named ultracomm.sln in your build folder.
   Visual Studio 2010 should open and you will see the ultracomm project
   in the Solution Explorer pane.

1. Right-click the ultracomm project, Properties > Configuration Properties > Linker >
   Additional Library Directories > Edit.... Add the location of the boost libraries,
   e.g. C:\local\boost_1_55_0\lib32-msvc-10.0. Click Apply.
   # TODO: this should be taken care of by CMake. Figure out how to add boost there.
 
1. From the "Build" menu select "Build Solution". This will compile ultracomm.
TODO   - Notice your build configuration. It is usually shown on the toolbar below the menu bar. It is set by default to Debug & Win32.
 

== Running

1. Copy these .dll files to the same folder as the executable. You should find them in the bin directory of your SDK folder.
    ulterius.dll (or ulterius_old.dll)
    umc_string_convert.dll
    umc_unit_convert.dll
    umc_xml.dll
    utx_utils.dll
    xerces-c_2_7_u_vc6.dll

TODO: see below


So far you have (hopefully) successfully "COMPILED" all the demo projects distributed with the sdk. However in order to successfully "RUN" them, you need to do some extra work:


12. Copy all the .dll files from the sdk's bin folder to your executables folder. For example, this would be from D:/sdk6.1.0/bin folder to D:/sdk6.1.0/stage/bin/Debug folder.
   - Notice that if your build configuration from step 11 was different, for example Release & Win32, then the folder would be D:/sdk6.1.0/stage/bin/Release

13. Copy all the .dll files from the Qt installation's bin folder to your executables folder. For example, this would be from C:/Qt/bin folder to D:/sdk6.1.0/stage/bin/Debug.
   - If the path to Qt binaries was added during Qt installation (Step 2), or you add it manually, to your environment variable "PATH" you don't need to go through this step.

Now you should be able to run any of the demo's either from within the Visual Studio environment, or from the bin folder like D:/sdk6.1.0/stage/bin/Debug  
   - Some executables like amplio console may need input arguments, for example file names, that you need to specify in order to run them successfully.

One last thing: The path to the firmware and settings are not correct. This is because we at Ultrasonix don't know where you will choose to build your projects (see step 4).
The default paths we have in the sdk demo's are based on the folder structure we have chosen for our distribution. In other words, if you want to run one of the pre-compiled
executables that come with the sdk, the paths to the firmware and configuration files are correct. These pre-compiled demos are in the bin folder of the sdk, for example
D:/sdk6.1.0/bin/*.exe. Now that your exectuables are in another folder, like D:/sdk6.1.0/stage/bin/Debug/*.exe then the relative paths are no longer correct.


14. If you are running a gui, change the path to the firmware and configuration to match your folder structure from the gui's menus. We have them in the menus for most of the demos.
   - If you are running a console demo, you may need to change the path in the source code and recompile in order to get the software fully functional.

