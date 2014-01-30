
#!/bin/bash
# load_data.sh

# This script file takes one password and links all the 
# data directories. It defaults to login as user cgrant.
# You would want to update it you should also make this script
# more general.
#
# >>  bash mount_data.sh "mypassword"
#
# Important! don't leak  your password, when executing this
# command prepend with a space so it is not saved in history.
#
# >> 


# Makes sure the folders exist /data/d0{1,2,3,4,5,6,7,8}/
# exist on this machine


echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d01 /data/d01
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d02 /data/d02
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d03 /data/d03
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d04 /data/d04
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d05 /data/d05
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d06 /data/d06
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d07 /data/d07
echo "$1" | sshfs -C -o idmap=user,follow_symlinks -o uid=1000 -o gid=1000 -o allow_other -o password_stdin cgrant@sm321-01.cise.ufl.edu:/data/d08 /data/d08
