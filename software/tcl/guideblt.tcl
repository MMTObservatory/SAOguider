
blt::vector xdata($graphlen)

for { set i 0 } { $i < $graphlen } { incr i } {
    set xdata($i) $i
}

proc seterror { err value index op } {
	upvar #0      $value v

catch {
    global	graphlen
    
	if { [${err}v length] >= $graphlen } {
		${err}v delete 0
	}
	${err}v append  $v
} reply; #puts $reply
}

proc settotal { err value index op } {

		upvar #0 ${err}errol e
		upvar #0 ${err}total t
		upvar #0      $value v

    if {![info exists t]} {set t 0}
    global	graphlen
    
	set e [format  "% 7.3f" $v]
        set t [format  "% 7.3f" [expr $t + $v]]
	global ${err}totv

	if { [${err}totv length] >= $graphlen } {
		${err}totv delete 0
	}
	${err}totv append  $t

	if { [${err}errv length] >= $graphlen } {
		${err}errv delete 0
	}
	${err}errv append  $e
}

proc mkgraphs { w typ } {
    grid \
	[mkgraph $w Error1 $typ Az]	\
	[mkgraph $w Error2 $typ El]
    grid \
	[mkgraph $w Error3 $typ Rot]	\
	[mkgraph $w Error4 $typ Focus]
}

proc mkboxgraphs { w typ n } {
    set boxes {}

    for { set box 1 } { $box <= $n } { incr box } {
	lappend boxes [mkgraph $w Box$box $typ Box$box]

    }

    eval grid $boxes
}

proc mkgraph { w err typ title} {
	set graph [blt::graph [entFixWidgetName $w.${err}graph]		\
			-height 2.25i -width 3i\
			-borderwidth  0		\
			-topmargin    20	\
			-bottommargin 15	\
			-rightmargin  5		\
			-leftmargin   40	\
			-plotborderwidth 0	\
			-title $title]

	$graph xaxis configure -title "" -color blue -ticklength 3
	$graph yaxis configure -title "" -color blue -ticklength .05i

	global ${err}${typ}v xdata
	blt::vector ${err}${typ}v 
	$graph element create ${err}${typ} -xdata xdata -ydata ${err}${typ}v

	$graph legend configure -hide yes

	#Blt_ZoomStack $graph

	return [entFixWidgetName $w.${err}graph]
}

