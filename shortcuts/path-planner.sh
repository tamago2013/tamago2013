#!/bin/bash

#==============================================
# visualizer , pathplannerの起動　( multiloggerはecho )
#==============================================


shell_home=${PWD}
cd ../
export tamago_home=${PWD}
cd $shell_home

#コンフィグディレクトリがないなら、作る
if [ ! -d ~/.config/terminator ] ; then
	mkdir ~/.config/terminator
fi

#すでに他のショートカットによってバックアップファイルが作成されているか？
if [ ! -f ~/.config/terminator/config.bk ] ; then
	make_buckup="1"
	echo "make TAMAGO terminator configure file."
else
	make_buckup="0"
	echo "TAMAGO terminator configure file already exists."
fi


#バックアップを作る(バックアップがまだない場合)
if [ ${make_buckup} = "1" ] ; then
	mv ~/.config/terminator/config ~/.config/terminator/config.bk
	cp terminator-config ~/.config/terminator/config
fi

#実行
terminator -l path-planner

#バックアップを自分で作ったときは、バックアップから元のコンフィグを復元
if [ ${make_buckup} = "1" ] ; then
	mv ~/.config/terminator/config.bk ~/.config/terminator/config	
	echo "reconstituted original terminator configure file."	
fi
