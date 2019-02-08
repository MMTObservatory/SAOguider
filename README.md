# SAOguider

SAOguider autoguiding system for the MMTO

To run the SAOguider software, you need three components - a camera server,
a guider server, and a gui.  The three components may run on three
different machines, or all on one machine, or any combination.  The host
and ports for the servers are found in our SVR DNS records.

The directory where all this software and this README resides, is specified
by the value of the environment variable, SAOGUIDER_ROOT.  If not present,
the value is set to /mmt/saoguider.  This is not needed to build the software,
just to run the servers, and guis.

There is a different gui for each instrument configuration.  The guis/
directory holds subdirectories for each of these.  Each instrument
configuration subdirectory has a startup script for the gui and a file of
parameters unique to that gui.  The startup script for the gui takes care of
setting all the unique parameters for that instrument configuration, but all
the startup scripts end up by executing the same ancient copy of ds9, which
then sources a large number of tcl files.  See software/NOTES.

There are currently three types of cameras used:  

    - camaz -- ITL's AZCAM controlled frame-transfer CCD guide cameras.
    - videoserver -- Dallan Porter's Axis videoserver networked framegrabber based video cameras.
    - zwo -- The ZWO ASI1600MM CMOS camera that replaced the videoscope intensified video camera.

Each camera type needs its own camera server and guider server.  The servers/
directory holds subdirectories for each camera type.  Each camera type
subdirectory has startup scripts for its servers, and files of parameters
unique to each server.  The file, servers/run_arch.csh, is used
to indicate whether to run the 32bit or 64bit version of these servers.
Note that each case, the guider server is really running the same executable,
just with different parameters.  Each gui will talk to only one set of
servers, depending of which camera type is used for that instrument
configuration.

The software/ directory holds all the software for the guis and servers.  It
contains the ancient copy of ds9 and all its tcl software.  This software is
all 32bit only.  This directory also holds all the source code for building
the servers.  You can build both 32bit and 64bit versions of the servers.
See software/NOTES.
