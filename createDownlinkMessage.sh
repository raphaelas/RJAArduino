source loRaWANProperties.sh
#ttn-lw-cli end-devices downlink replace $APPLICATIONID $DEVICEID --frm-payload 48656c4c6f526120576f726c6421 --f-port 1
ttn-lw-cli end-devices downlink replace $APPLICATIONID $DEVICEID --frm-payload 31 --f-port 1
ttn-lw-cli end-devices downlink list $APPLICATIONID $DEVICEID
