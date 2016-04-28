# cyclops
In order to compile this code please follow these steps:

1)  create an area for building the code.  It is useful to make a logical variable (e.g. $MYBUILD) pointing to the build area.
2)  create an area for holding the output of your compilation (shared objects, include files, etc...).  It is useful to make a variable (e.g. $MYINSTALL) pointing to that area.
3)  Update your $LD_LIBRARY_PATH as shown below (e.g. in a .bashrc or other login command file):
 export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
the above line pre-pends the new install area to your existing path.

The following steps should lead to working code:

cd $MYBUILD
<path_to_cyclops>/autogen.sh --prefix=$MYINSTALL
make install

Good Luck!

==================

Required external code:  root (http://root.cern.ch), pmonitor (purschke@bnl.gov)