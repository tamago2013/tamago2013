#!/bin/bash

#==============================================
# visualizer , pathplannerの起動　( multiloggerはecho )
#==============================================


shell_home=${PWD}
cd ../
export tamago_home=${PWD}
cd $shell_home

mv ~/.config/terminator/config ~/.config/terminator/config.bk
cp terminator-config ~/.config/terminator/config
terminator -l VLP
mv ~/.config/terminator/config.bk ~/.config/terminator/config	
