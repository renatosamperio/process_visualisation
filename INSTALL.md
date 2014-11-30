sudo apt-get install mesa-common-dev cmake-curses-gui qt5-default qttools5-dev-tools libboost-thread-dev libboost-system-dev python-setuptools mercurial

sudo apt-get install python-pip python-dev build-essential python-psutil 
sudo pip install --upgrade pip 

hg clone https://code.google.com/p/psutil/
cd psutil
sudo python setup.py install
