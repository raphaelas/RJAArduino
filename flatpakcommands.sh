flatpak-builder dist com.raphaelastrow.timeuntilalarm.yml
flatpak-builder --user --install --force-clean dist com.raphaelastrow.timeuntilalarm.yml
flatpak run com.raphaelastrow.timeuntilalarm
