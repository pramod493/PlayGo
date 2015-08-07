cd ~/Qt-Build/PlayGo-Debug/

qmake ~/Dropbox/CDI_PlayGo-Final/CDI_PlayGo.pro -r -spec linux-g++ CONFIG+=debug

make -j3
make
