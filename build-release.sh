cd ~/Qt-Build/PlayGo-Release/

qmake ~/Dropbox/CDI_PlayGo-Final/CDI_PlayGo.pro -r -spec linux-g++ CONFIG+=release

make -j3
make

./PlayGo
