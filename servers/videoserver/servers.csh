if ($SAOGUIDER_ROOT == /mmt/saoguider) then
    setenv CAMERA_VIDEOSERVER `/mmt/bin/mmtservdata camera-videoserver | tr " " ":"`
    setenv VIDEOSERVER_IMAGEIO `/mmt/bin/mmtservdata camera-videoserver-imageio | tr " " ":"`
    setenv GUIDSERV `/mmt/bin/mmtservdata guider-videoserver | tr " " ":"`
    setenv TELESCOPE `/mmt/bin/mmtservdata telserver | tr " " ":"`
else
# testing
    setenv CAMERA_VIDEOSERVER 127.0.0.1:3012
    setenv VIDEOSERVER_IMAGEIO 127.0.0.1:3013
    setenv GUIDSERV 127.0.0.1:3010
    setenv TELESCOPE 127.0.0.1:5403
endif
#setenv VIDEOSERVER videoserver1.mmto.arizona.edu
setenv VIDEOSERVER `dig +short videoserver1.mmto.arizona.edu`
setenv GUIDECAM $CAMERA_VIDEOSERVER
setenv IMAGEIOPORT $VIDEOSERVER_IMAGEIO
setenv FOCUS $TELESCOPE;setenv GUIDE $GUIDSERV;unsetenv UPARM;unsetenv MSGDEBUG
source $SAOGUIDER_ROOT/servers/run_arch.csh
