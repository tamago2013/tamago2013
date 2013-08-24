#!/bin/bash

#==============================================
# particle-localizerとopsm-particle-evaluatorの起動
#==============================================


shell_home=${PWD}
cd ../
export tamago_home=${PWD}
cd $shell_home

mv ~/.config/terminator/config ~/.config/terminator/config.bk
cp terminator-config ~/.config/terminator/config
terminator -l localizer
mv ~/.config/terminator/config.bk ~/.config/terminator/config	
