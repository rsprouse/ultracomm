ultracomm
=========

A command line utility for communicating with and acquiring data from an Ultrasonix ultrasound machine.

The current release code is ALPHA.


Prerequisites:
--------------


 - Visual Studio 12 2013. Should also compile with Visual Studio 2010 Express Edition from Microsoft,
   Service Pack 1 or greater.

 - boost (boost.org) libraries, especially program options library.

 - CMake from KitWare.

 - Ultrasonix SDK to match your version of Exam.


Compiling
---------

1. Edit the CMakeLists.txt file in the root directory of the ultracomm project.
   Find the section for 'Variables that might need to be changed' and see
   comments embedded there for details of what needs attention.

1. Create a build folder wherever you like on your system.

1. Run the CMake gui.

1. Provide the absolute path to the project root in the "Where is the source code:" 
   box, e.g. `C:\src\ultracomm`.

1. Provide the absolute path to your build folder in the "Where to build the
   binaries:" box, e.g. `C:\build\ultracomm`.

1. Press the Configure button. Select the "Visual Studio 12 2013" as the
   "generator for this project". Use the "Select default native compilers"
   option.

1. Press Generate. 

1. Look for the file named `ultracomm.sln` in your build folder.
   Open it with Visual Studio 2013. You will see the ultracomm project
   in the Solution Explorer pane.

1. (Optional) Change the active configuration to 'Release'.

1. Right-click the ultracomm project and choose 'Build'. This will compile `ultracomm.exe`.

1. Take note of your build configuration, usually shown on the toolbar below the menu bar.
   It is set by default to Debug & Win32. This will help you find the compiled executable.
 

Running
-------

1. Find the compiled executable `ultracomm.exe` in a subdirectory of the
   `bin` directory of your build folder. The name of the subdirectory will
   depend on your build configuration, for example, `Debug` or `Release`.
   If you build directory is `C:\build\ultracomm` then your executable should
   be in a directory named like `C:\build\ultracomm\bin\Debug` or
   `C:\build\bin\Release`.
1. If necessary, copy these .dll files from the Ultrasonix SDK to the same
   folder as the executable. It appears that this step is required for SDK 5.7.4
   but not 6.x.
    `ulterius.dll` (or `ulterius_old.dll`)
    `umc_string_convert.dll`
    `umc_unit_convert.dll`
    `umc_xml.dll`
    `utx_utils.dll`
    `xerces-c_2_7_u_vc6.dll`


