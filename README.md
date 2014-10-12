openDMD
======

Brief Introduction
------------------
        openDMD(open Distributed Motion Detection) is a motion detection   
        program for distributed multi-point monitoring. openDMD can work
        in 2 differend mode: picture mode and video mode, which captures
        picture or record video, respectively, or do both at same time.
        openDMD works at X86-64/Linux platform now, while working at ARM
        embedded system is ultimate goal. 

Features
--------
        1. Work in 2 mode: picture mode, video mode;
        2. work as a singleton or in a multi-node cluster;
        3. Persistently store monitoring result(jpeg or flv);
        4. Persistently record motion event to database;
        5. Web administration to control system running;

Download && Config && Build && Install
--------------------------------------
        git clone https://github.com/weizhenwei/openDMD.git
        cd openDMD
        ./autogen.sh  (optional: for regenerate configure file)
        ./configure [options]  (run ./configure --help for more information)
        make
        make install


Run
---
        ./opendmd -f opendmd.cfg


About config file
-----------------
        openDMD config file controls the program running; Examples of
        config file can be found at directory ./doc/config in source code.
        These examples are self-explanatory, so a proper config file can be
        easily written.

Acknowledge
-----------
        The main idea of openDMD is inspired by the project Motion at
            https://github.com/sackmotion/motion.git
        
        The openDMD uses the following libraries:
            libjpeg: http://www.ijg.org/files/jpegsrc.v9a.tar.gz
            libortp: http://download.savannah.gnu.org/releases/linphone/
                     ortp/sources/ortp-0.22.0.tar.gz
            libsqlite: http://www.sqlite.org/2014/
                       sqlite-amalgamation-3080500.zip
            libx264: ftp://ftp.videolan.org/pub/videolan/x264/snapshots/
                     x264-snapshot-20140525-2245-stable.tar.bz2
        
        The jit part of openDMD originates from Qemu-2.0.0:
            http://wiki.qemu-project.org/download/qemu-2.0.2.tar.bz2
