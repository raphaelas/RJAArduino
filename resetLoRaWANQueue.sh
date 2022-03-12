source loRaWANProperties.sh
ttn-lw-cli end-devices downlink list $APPLICATIONID $DEVICEID
ttn-lw-cli end-devices downlink clear $APPLICATIONID $DEVICEID
ttn-lw-cli end-devices downlink list $APPLICATIONID $DEVICEID
