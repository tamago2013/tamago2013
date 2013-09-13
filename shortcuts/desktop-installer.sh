#!/bin/bash

#================================
# ショートカットアイコンをデスクトップランチャーにする
#================================

#----------------------------------------------
# ./config/tamagoの作成
if [ ! -d ~/.config/tamago ] ; then
	mkdir ~/.config/tamago
fi

#----------------------------------------------
# tamagoホームディレクトリの場所の取得
cd ../
tamago_home=${PWD}

#----------------------------------------------
# ./config/tamago内に各種ファイルを作成

#home-path.sh(tamagoのホームディレクトリの場所を書いたシェル)を作成
echo "tamago_home="${tamago_home} > ~/.config/tamago/home-path.sh
chmod a+x ~/.config/tamago/home-path.sh

#シェル・アイコンのコピーを作成
cd ~/.config/tamago/
cp -fr ${tamago_home}/shortcuts/bin ~/.config/tamago/bin
cp -fr ${tamago_home}/shortcuts/icons ~/.config/tamago/icons

#-----------------------------------------------
#デスクトップランチャを作成
cd ~/Desktop

function make_desktop_launcher()
{
	echo "[Desktop Entry]" > ~/Desktop/$1.desktop
	echo "Type=Application" >> ~/Desktop/$1.desktop
	echo "Name="$1 >> ~/Desktop/$1.desktop
	echo "GenericName=team tamagokake-gohan 2013 , desktop launcher ("$1")" >> ~/Desktop/$1.desktop
	echo "Icon="${HOME}"/.config/tamago/icons/"$1"-icon.png" >> ~/Desktop/$1.desktop
	echo "Exec="${HOME}"/.config/tamago/bin/"$1".sh" >> ~/Desktop/$1.desktop
	echo "Terminal=false" >> ~/Desktop/$1.desktop

	chmod a+x $1.desktop
}

make_desktop_launcher joystick
make_desktop_launcher localizer
make_desktop_launcher multi-logger
make_desktop_launcher path-planner
make_desktop_launcher platform
make_desktop_launcher ssm-killer
make_desktop_launcher urg
make_desktop_launcher visualizer









