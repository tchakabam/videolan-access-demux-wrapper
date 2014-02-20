videolan-access-demux-wrapper
=============================

A wrapper to easily implement an VideoLAN player so called "access demux module" plugin based on a light-weight multimedia framework using Poco and modern C++

Dependencies: POCO lib', STL

Basically, just write an implementation against the disparat::Stream interface, and check out the VLC plugin example code.

If you have questions, send me a mail. Have fun :)

Note: Not providing any compilation scripts right now to hang this into the VLC tree, or any kind of factory for the interface implementations to be hooked up automatically with the plugin, but ... might come up with this in the future for this project :)
