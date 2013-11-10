#!/bin/bash

#================================
# デスクトップランチャとその関係ファイルを削除
#================================

#現役のランチャ名
rm -f ~/Desktop/joystick.desktop
rm -f ~/Desktop/localizer.desktop
rm -f ~/Desktop/multi-logger.desktop
rm -f ~/Desktop/main-controller.desktop
rm -f ~/Desktop/human-detector.desktop
rm -f ~/Desktop/platform.desktop
rm -f ~/Desktop/ssm-killer.desktop
rm -f ~/Desktop/urg.desktop
rm -f ~/Desktop/visualizer.desktop
rm -f ~/Desktop/ptz.desktop

#古いランチャ名
rm -f ~/Desktop/path-planner.desktop

#tamagoディレクトリを削除
rm -fr ~/.config/tamago

