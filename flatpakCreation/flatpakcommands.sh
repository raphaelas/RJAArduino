MYNAME=raphaelastrow
rm -r dist
flatpak-builder dist com.$MYNAME.timeuntilalarm.yml
flatpak-builder --user --install --force-clean dist com.$MYNAME.timeuntilalarm.yml
flatpak run com.$MYNAME.timeuntilalarm
