Launch the script ./initialisation.sh
Sample script: ./simulation.sh

ccmake :
sudo apt-get install cmake-curses-gui

octave :
sudo apt install octave
package of octave :
sudo apt install octave-bim octave-communications octave-communications-common octave-control octave-data-smoothing octave-dataframe octave-econometrics octave-fpl octave-ga octave-general octave-geometry octave-gsl octave-image octave-interval octave-linear-algebra octave-ltfat octave-ltfat-common octave-miscellaneous octave-missing-functions octave-msh octave-nan octave-nurbs octave-ocs octave-octcdf octave-odepkg octave-optim octave-optiminterp octave-parallel octave-quaternion octave-signal octave-sockets octave-specfun octave-splines octave-statistics octave-stk octave-strings octave-struct octave-tsa octave-zenity
sudo octave -qf --eval "pkg rebuild -auto, pkg rebuild -noauto tsa sockets ocs nan"
