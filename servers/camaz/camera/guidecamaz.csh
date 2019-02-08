#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/camaz/servers.csh
setenv MSGINITDIR $SAOGUIDER_ROOT/servers/camaz/camera
exec $SAOGUIDER_ROOT/software/bin$TARGET_ARCH/guidecamaz
