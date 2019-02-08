#!/data/mmti/bin/tclsh

proc WCSAdjust {} {

    global regions_old
    global current

    $current(frame) marker delete all
    UA2Overlay

    execbg overlaydone -output regions_old xpaget $::ds9(title) regions -system wcs
    vwait overlaydone

    set w .wcsadjust
    toplevel $w
    set message "Move at least 2 regions on top of stars.  Then click Continue"
    grid [message $w.msg -text $message] -sticky news
    grid [frame $w.b] - -sticky new
    grid [button $w.b.ok     -text OK     -command wcsadjustok]       \
	 [button $w.b.cancel -text Cancel -command wcsadjustcancel]	\
 	 -sticky news
}

proc wcsadjustcancel {} {
    destroy .wcsadjust
}

proc wcsadjustok {} {
    destroy .wcsadjust
    global regions_old regions_new

    execbg overlaydone -output regions_new xpaget $::ds9(title) regions -system wcs
    vwait overlaydone

#    set regions_old [read [open /home/bmcleod/ds9tools/z1 r]]
#    set regions_new [read [open /home/bmcleod/ds9tools/z2 r]]

    set ra0 [miniKWLookup CRVAL1]
    set dc0 [miniKWLookup CRVAL2]
    set c11 [miniKWLookup CD1_1]
    set c12 [miniKWLookup CD1_2]
    set c21 [miniKWLookup CD2_1]
    set c22 [miniKWLookup CD2_2]
    set cosd [ expr cos($ra0 / 180. * 3.141592654) ]

    set    table "RA1\tDC1\tRA2\tDC2\n"
    append table "---\t---\t---\t---\n"

    foreach s1 [split $regions_new \n] s2 [split $regions_old \n] {

	set l1 [split $s1 "(,"]
	set l2 [split $s2 "(,"]

	set ra1 [lindex $l1 1]
	set dc1 [lindex $l1 2]

	set ra2 [lindex $l2 1]
	set dc2 [lindex $l2 2]

	if { ($ra1 != $ra2) || ($dc1 != $dc2) } {
	    puts "$ra0 $ra1 $ra2 $dc0 $dc1 $dc2"
	    set dra1 [expr ($ra1 - $ra0) * $cosd ]
	    set ddc1 [expr $dc1 - $dc0]
	    set dra2 [expr ($ra2 - $ra0) * $cosd ]
	    set ddc2 [expr $dc2 - $dc0]
	    append table [format "%10.7f\t%10.7f\t%10.7f\t%10.7f\n"  \
		    $dra1   $ddc1   $dra2   $ddc2   ]
	}
    }
    puts $table
    set fitresults [exec xyfittable -3 RA1 DC1 RA2 DC2 << $table]
    puts $fitresults

    foreach line [split $fitresults \n] {
	set line [split $line "\t"]
	if { [llength $line] >= 2 } {
	    set [lindex $line 0] [lrange $line 1 end]
	}
    }
    foreach {raoff a11 a12 dcoff a21 a22 } $Coeff {}

    set CRVAL1 [expr $ra0 - $raoff / $cosd]
    set CRVAL2 [expr $dc0 - $dcoff]
    set CD1_1  [expr ($c11 * $a11) + ($c12 * $a21)]
    set CD1_2  [expr ($c11 * $a12) + ($c12 * $a22)]
    set CD2_1  [expr ($c21 * $a11) + ($c22 * $a21)]
    set CD2_2  [expr ($c21 * $a12) + ($c22 * $a22)]

    puts "OLD"
    foreach keyword { ra0 dc0 c11 c12 c21 c22 } {
	puts "$keyword\t[set $keyword]"
    }

    puts "NEW"
#    foreach keyword { CRVAL1 CRVAL2 CD1_1 CD1_2 CD2_1 CD2_2 } {
#	puts "msccmd \"hedit \$input $keyword [set $keyword] delete- update+ verif-\""
#    }

    set dra [expr -($RA2Zero * 3600.)]
    set ddc [expr -($DC2Zero * 3600.)]

    puts "fixmosaic fixmos- fixrot+ fixcen+ rotcorr=$Orient deltara=$dra deltadc=$ddc"

}
