# ####
#
# starbase.tcl -- Tcl interface to starbase
#
# ####

# Starbase Tables Interface
#

set starbasedebug 0

proc Starbase {} {
	global  Starbase
	return $Starbase(version)
}

set Starbase(version) "Starbase Tcl Driver 1.0"

proc starbase_nrows   { D } 	        { upvar $D T; return $T(Nrows) 	}
proc starbase_ncols   { D } 	        { upvar $D T; return $T(Ncols) 	}
proc starbase_get     { D row col }     { upvar $D T; return $T($row,$col) }
proc starbase_set     { D row col val } { upvar $D T; set T($row,$col) $val; 	}
proc starbase_colname { D col  }        { upvar $D T; set row 0; 
						return $T($row,$col)}
proc starbase_columns { D      }        { upvar $D T; return $T(Header) }
proc starbase_colnum  { D name }        { upvar $D T; if { [catch { set num $T($name) }] } {
            set num 0 }; return $num
}

proc starbase_init { t } {
	upvar $t T

	set T(Nrows) 0
	set T(Ncols) 0
	set T(Header) ""
}

# Set up a starbase data array for use with ted
#
proc starbase_driver { Dr } {
	upvar $Dr driver

	set driver(nrows)	starbase_nrows
	set driver(ncols)	starbase_ncols
	set driver(get)		starbase_get
	set driver(set)		starbase_set
	set driver(colname)	starbase_colname
	set driver(colnum)	starbase_colnum
	set driver(columns)	starbase_columns
	set driver(colins)	starbase_colins
	set driver(coldel)	starbase_coldel
	set driver(colapp)	starbase_colapp
	set driver(rowins)	starbase_rowins
	set driver(rowdel)	starbase_rowdel
	set driver(rowapp)	starbase_rowapp
}
starbase_driver Starbase

proc starbase_new { t cols } {
	upvar $t T

    set T(Header) [join $cols \t]
    set T(H_1)    $T(Header)

    regsub -all -- {[^ \t]} $T(Header) - T(Dashes)
    set T(H_2)    $T(Dashes)

    set T(Ncols) [llength $cols]
    set T(Ndshs)  [llength $T(Header)]
    set T(HLines) 2
    starbase_colmap T

    set T(Nrows) 0
}

proc starbase_hdrcpy { t c } {
    	upvar $t T
	upvar $c C

    array set C [array get T H_*]
    array set C [array get T N_*]
    set C(HLines) T(HLines)
}

proc starbase_colmap { t } {
	upvar $t T

    set r 0
    set c 0	
    foreach column $T(Header) {
	incr c
	set column [string trim $column]
	set T($column) $c
	set T($r,$c) $column
    }
    set T(Ncols) $c
}

proc starbase_coldel { t here } {
	upvar $t T

    set Ncols $T(Ncols)

    set T(Header) [lreplace $T(Header) [expr $here - 1] [expr $here - 1]]
    set T(Dashes) [lreplace $T(Dashes) [expr $here - 1] [expr $here - 1]]
    starbase_colmap T

    for { set row 1 } { $row <= $T(Nrows) } { incr row } {
        for { set col $here } { $col < $Ncols } { incr col } {
	    if { [catch { set val $T($row,[expr $col + 1]) }] } {
		set T($row,$col) ""
	    } else {
	        set T($row,$col) $val
	    }
	}
    }
}

proc starbase_colapp { t name { here -1 } } {
	upvar $t T

	if { $here == -1 } {
		set here $T(Ncols)
	}
	starbase_colins T $name [expr $here + 1]
}

proc starbase_colins { t name here } {
	upvar $t T

    if { [info exists T(Header)] == 0 } {
	set T(Header) $name
    } else {
        set T(Header) [linsert $T(Header) [expr $here - 1] $name]
    }
    starbase_colmap T

    set Nrows $T(Nrows)
    for { set row 1 } { $row <= $Nrows } { incr row } {
        for { set col $T(Ncols) } { $col > $here } { incr col -1 } {
	    if { [catch { set val $T($row,[expr $col - 1]) }] } {
		set T($row,$col) ""
	    } else {
	        set T($row,$col) $val
	    }
	}
    }

    for { set row 1 } { $row <= $Nrows } { incr row } {
	set T($row,$here) ""
    }

    return $here
}

proc starbase_header { t fp } {
	upvar $t T
	global starbase_line
	set N 1

    array unset T
    if { [info exists starbase_line] } {
	set line $starbase_line
	set n 1

	set T(H_$n) $line
	if { [regexp -- {^ *(-)+ *(\t *(-)+ *)*} $line] } break
	if { $n >= 2 } {
	    set ind [string first "\t" $T(H_[expr $n-1])]
	    if { $ind >= 0 } {
		set name [string trim [string range  $T(H_[expr $n-1]) 0 [expr $ind - 1]]]
		incr ind
		set T(H_$name) [string range $T(H_[expr $n-1]) $ind end]
		set T(N_$name) [expr $n-1]
	    }
	}

	unset starbase_line
	set N 2
    }
    for { set n $N } { [set eof [gets $fp line]] != -1 } { incr n } {
	set T(H_$n) $line
	if { [regexp -- {^ *(-)+ *(\t *(-)+ *)*} $line] } break

	if { $n >= 2 } {
	    set ind [string first "\t" $T(H_[expr $n-1])]
	    if { $ind >= 0 } {
		set name [string trim [string range  $T(H_[expr $n-1]) 0 [expr $ind - 1]]]
		incr ind
		set T(H_$name) [string range $T(H_[expr $n-1]) $ind end]
		set T(N_$name) [expr $n-1]
	    }
	}
    }

    if { $eof == -1 } {
	error "ERROR: in starbase_header: unexpected eof"
    }

    set T(H_$n) $line
    set T(HLines) $n

    set T(Header) [split $T(H_[expr $n-1])  "\t"]
    set T(Dashes) [split $T(H_$n)	    "\t"]
    set T(Ndshs)  [llength $T(Dashes)]

    starbase_colmap T

    return $T(Header)
}

proc starbase_hdrget { t name } {
    upvar $t T

    return $T(H_$name)
}

proc starbase_hdrset { t name value } {
    upvar  $t T 

    if { ![info exists T(H_$name)] } {
	set n [incr T(HLines)]

	set T(H_[expr $n-0]) $T(H_[expr $n-1])
	set T(H_[expr $n-1]) $T(H_[expr $n-2])
	set T(N_$name) [expr $n-2]
    }
    set T(H_$name) 	  $value
    set T(H_$T(N_$name)) "$name	$value"
}

proc starbase_hdrput { t fp } {
	upvar $t T

    if { [info exists T(HLines)] && ($T(HLines) != 0)  } {
	    set nl [expr $T(HLines) - 2]
	    for { set l 1 } { $l <= $nl } {  incr l } {
		puts $fp $T(H_$l)
	    }
    }

    if { ![info exists T(Ncols)] || ($T(Ncols) == 0)  } {
	return
    }

    set r 0
    set Ncols $T(Ncols)
    for { set c 1 } { $c <= $Ncols } {  incr c } {
	puts -nonewline $fp "$T($r,$c)"
	if { $c != $Ncols } {
	    puts -nonewline $fp "\t"
	} else {
	    puts -nonewline $fp "\n"
	}
    }

    for { set c 1 } { $c <= $Ncols } {  incr c } {
	set len [string length $T($r,$c)]
	for { set d 1 } { $d <= $len } { incr d } {
	    puts -nonewline $fp "-"
	}
	if { $c != $Ncols } {
	    puts -nonewline $fp "\t"
	} else {
	    puts -nonewline $fp "\n"
	}
    }
}

proc starbase_readfp { t fp } {
	upvar $t T

    starbase_header T $fp

    set NCols [starbase_ncols T]

    for { set r 1 } { [gets $fp line] != -1 } { incr r } {
	if { [string index $line 0] == "\f" } {
	    global starbase_line
	    set starbase_line [string range $line 1 end]
	    break
	}
	set c 1
	foreach val [split $line "\t"] {
	    set T($r,$c) [string trim $val]
	    incr c
	}
	for { } { $c <= $NCols } { incr c } {
	    set T($r,$c) {}
	}
    }
    set T(Nrows) [expr $r-1]
}

proc starbase_read { t file } {
	upvar $t T

    set fp [open $file]
    starbase_readfp T $fp
    close $fp

    set T(filename) $file
}

proc starbase_writefp { t fp } {
	upvar $t T

    starbase_hdrput T $fp

    if { ![info exists T(Nrows)] || ($T(Nrows) == 0)  } {
	return
    }

    set Nrows $T(Nrows)
    set Ncols $T(Ncols)
    for { set r 1 } { $r <= $Nrows } {  incr r } {
	for { set c 1 } { $c < $Ncols } {  incr c } {
	    if { [catch { set val $T($r,$c) }] } {
		    set val ""
	    }

	    if { [regexp {[\n\t]} $val] } {
		error "Table cell $r,$c contains a newline or tab: $val"
	    }

	    puts -nonewline $fp "$val	"
	}
	if { [catch { set val $T($r,$c) }] } {
		set val ""
	}

	if { [regexp {[\n\t]} $val] } {
	    error "Table cell $r,$c contains a newline or tab: $val"
	}

	puts $fp $val
    }
}

proc starbase_write { t file } {
	upvar $t T

    set fp [open $file w]
    fconfigure $fp -translation { auto lf }
    starbase_writefp T $fp
    close $fp
}

proc starbase_rowapp { t { row -1 } { rowval {} } } {
		upvar $t T

	if { $row == -1 || $row == "" } {
		set row $T(Nrows)
	}
	starbase_rowins T [expr $row + 1] $rowval
}

proc starbase_rowins { t row { rowval {} } } {
		upvar $t T

	if { $row == "" } {
	    set row 1
	}

	incr T(Nrows)

	set Nrows $T(Nrows)
	set Ncols $T(Ncols)
	for { set r $Nrows } { $r > $row } { set r [expr $r-1] } {
	    for { set c 1 } { $c <= $Ncols } { incr c } {
		if { [catch { set val $T([expr $r-1],$c) }] } {
			set val ""
		}
			
	        set T($r,$c) $val
	    }
	}

	starbase_rowset T $r $rowval
}

proc starbase_rowset { t r { rowval {} } } {
    upvar $t T

	set Ncols $T(Ncols)
	for { set c 1 } { $c <= $Ncols } { incr c } {
	    set T($r,$c) [lindex $rowval [expr $c - 1]]
	}
}

proc starbase_rowget { t r } {
    upvar $t T

	set Ncols $T(Ncols)
	for { set c 1 } { $c <= $Ncols } { incr c } {
	    lappend row $T($r,$c)
	}

    return $row
}

proc starbase_rowdel { t { row -1 } } {
	upvar $t T

    if { $row == "" } 	    { set row -1 }
    if { $T(Nrows) <= 0   } { return 	 }
    if { $row > $T(Nrows) } { return 	 }

    incr T(Nrows) -1

    set Nrows $T(Nrows)
    set Ncols $T(Ncols)
    for { set r $row } { $r <= $Nrows } { incr r } {
	for { set c 1 } { $c <= $Ncols } { incr c } {
	    if { [catch { set val $T([expr $r+1],$c) }] } {
		    set val ""
	    }
		    
	    set T($r,$c) $val
	}
    }

    for { set c 1 } { $c <= $Ncols } { incr c } {
	set T($r,$c) ""
    }
}

proc starbase_httpreader { t wait sock http } {
	global $t
        global starbasedebug
	upvar #0 $wait W
	upvar #0 $t T

    set T(http) $http

    if { ![info exists T(state)]  } {
      error "ERROR: starbase_httpreader not properly initialized"
    }

    switch -- $T(state) {
	0 {
	    fconfigure $sock -blocking 1
	    set T(state)	1
	    set T(Nrows)	0
	    set T(HLines)	0
	}

	1 {
	    incr ${t}(HLines)
	    set n $T(HLines)
		
	    if { [gets $sock line] == -1 } {
		set T(state) -1
		set T(HLines) [expr $T(HLines) - 1]
		set T(Nrows) 0
		return
	    }

	    if { $n >= 2 } {
		set ind [string first "\t" $T(H_[expr $n-1])]
		if { $ind >= 0 } {
		    set name [string trim [string range  $T(H_[expr $n-1]) 0 [expr $ind - 1]]]
		    incr ind
		    set T(H_$name) [string range $T(H_[expr $n-1]) $ind end]
		    set T(N_$name) [expr $n-1]
		}
	    }

	    set T(H_$n) $line

	    if { [regexp -- {^ *(-)+ *(\t *(-)+ *)*} $line] } {
		set T(Header) [split $T(H_[expr $n-1])  "\t"]
		set T(Dashes) [split $T(H_$n)		"\t"]
		set T(Ndshs)  [llength $T(Dashes)]
		
		starbase_colmap T
		set T(state) 2
	    }
	}

	2 {
	    if { [gets $sock line] == -1 } {
		set T(state) 0
	    } else {
		if { $starbasedebug } { 
		    puts [format "starbase_httpreader: %s" $line]
		}
		incr ${t}(Nrows)
		set r $T(Nrows)
		
		set NCols [starbase_ncols T]
		set c 1
		foreach val [split $line "\t"] {
		    set T($r,$c) $val
		    incr c
		}
		for { } { $c <= $NCols } { incr c } {
		    set T($r,$c) {}
		}
	    }
	}

	default {
	    error "ERROR: unknown switch in starbase_httpreader"
	}
    }
}

proc starbase_cancel { t wait http } {
	upvar #0 $wait W
	upvar #0 $t T

	set W 1
}

proc starbase_http { t url wait } {
    upvar #0 $t T

    set T(state) 0
    set T(http) [http::geturl $url 				\
		-handler [list starbase_httpreader $t $wait] 	\
		-command [list starbase_cancel $t $wait]]
}

proc starbase_httpkill { t } {
	upvar #0 $t T

    http::reset $T(http)
}

proc starbasetolist { t } {
	upvar $t T

    for { set j 0 } { $j <= $T(Nrows) } { incr j } {
        for { set i 1 } { $i <= $T(Ncols) } { incr i } {
	    lappend Col$j $T($j,$i)
        }
	lappend Cols [set Col$j]
    }

    return $Cols
}

proc listtostarbase { t l } {
	upvar $t T

	set r 0
	foreach row $l {
	    set c 0
	    foreach col $row {
		incr c
		set T($r,$c) $col
	    }
	    incr r
	}

	set T(Nrows) [expr $r -1]
	set T(Ncols) $c

	return $t
}

proc sorttable { t columns { s All } { functions {} } args } {
	upvar $t T

    if { ![string compare $s {}] } { return {} }


    if { ![string compare $s All] } {
	# Make a selection list
	#
	set s {}
	for { set j 1 } { $j <= $T(Nrows) } { incr j } {
	    lappend s $j
	}
    }

    # Make a sort list
    #
    foreach row $s {
	set l {}
	foreach $col $columns \
		$fun $function {
	    lappend l $row
            set c [starbase_colnum T $col]

	    if { ![string compare $fun {}] } {
	        lappend l $T($row,$c)
	    } else {
		lappend l [eval $fun $T($row,$c)]
	    }
	}
	lappend L $l
    }

    set s {}
    set L [eval lsort -index 1 $args \$L]
    foreach row $L {
	lappend s [lindex $row 0]
    }
    return $s
}

# Example
#
# set col [starbase_colnum Exp CatalogRow]
# set colrow guy
# 
# set selectexpr [subst { expr \$T(\$row,$col) == $catrow }]
# set rowlist [selecttable Exp $selectexpr]

proc selecttable { t expr { s All } } {
	upvar $t T

    if { ![string compare $s All] } {
	# Make a selection list
	#
	set s {}
	for { set j 1 } { $j <= $T(Nrows) } { incr j } {
	    lappend s $j
	}
    }

    set L {}
    foreach row $s {
	if { [eval $expr] } { lappend L $row }
    }

    return $L
}

proc starbase_foreachrow { args } {
    set T [lindex $args 0]

    if { [llength $args] == 2 } {
	set body [lindex $args 1]
	set var   row
    } else {
	set body [lindex $args 2]
	set var  [lindex $args 1]
    }

    uplevel [subst {
	for { set $var 1 } { \$$var <= \${${T}(Nrows)} } { incr $var } {
	$body
    } }]
}

proc starbase_foreachcol { args } {
    set T [lindex $args 0]

    if { [llength $args] == 2 } {
	set body [lindex $args 1]
	set var   col
    } else {
	set body [lindex $args 2]
	set var  [lindex $args 1]
    }

    uplevel [subst {
	for { set $var 1 } { \$$var <= \${${T}(Ncols)} } { incr $var } {
	$body
    } }]
}
