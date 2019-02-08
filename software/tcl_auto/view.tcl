# The view is a list of lists which maps table columns to data array, data
# column name pairs.
#
#	Where Catalog and Fibers are global arrays containing table
#	drivers a view might look like this:
#
#	set view { { Catalog RA }
#		   { Catalog Dec }
#		   { Catalog Mag }
#		   { Fibers  X }
#		   { Fibers  Y }
#		}
#
proc tab_viewsetnrows { V nrows } {
    upvar #0 $V view
    set view(rows)   $nrows
}

proc tab_viewcreate { V viewlist nrows } {
	upvar #0 $V view

    set view(rows)   $nrows
    set view(view)   $viewlist
    set view(Header) {}

    tab_viewproject view $viewlist
}

proc tab_viewselect { V rowslist } {
	upvar $V view
	set view(selection) $rowslist
}

proc tab_viewproject { V viewlist } {
	upvar $V view

    set i 1
    foreach c $viewlist {
	set dat [string trim [lindex $c 0]]
	set col [string trim [lindex $c 1]]


	if { ![string compare [string index $dat 0] "!"] } {	# Skip commands
	    set view(0,$i) $col	
	    set view($col) $i
	    lappend view(Header) $col
	    lappend vlist [list $dat $col]
	    incr i
	    continue
	}

	lappend tables $dat

	if { [llength $c] >= 3 } {
	    set name [string trim [lindex $c 2]]
	} else {
	    set name $col
	}

	upvar #0 $dat D
	set map [$D(colnum) D $col]

	lappend view(Header) $name
	set view($name)      $i
	set view(0,$i)       $name	

	lappend vlist "$dat $col $name $map"
	incr i
    }

    set view(view) $vlist
    set view(tables) [lsort -u $tables]
}

proc tab_viewlist { t } {
	upvar $t T

    set viewlist {}
    foreach c [starbase_columns T] {
	lappend viewlist "$t [string trim $c]"
    }

    return $viewlist
}

proc tab_viewcolumns { V } 	  { upvar $V view;  return $view(Header) }

proc tabview_mapdat  { view col } {		# View column to data array
	lindex [lindex $viewlist [expr $col-1]] 0
}
proc tabview_mapcol  { view col } {		# View column to data column
	lindex [lindex $viewlist [expr $col-1]] 3
}


# Table driver wrappers
#
proc tab_viewnrows  { V }		{ upvar $V view; return $view(rows)		}
proc tab_viewncols  { V }		{ upvar $V view; llength $view(view)		}
proc tab_viewset    { V row col value }	{
	upvar $V view;
	set viewlist $view(view)

	set dattab [lindex [lindex $viewlist [expr $col-1]] 0]
	set datcol [lindex [lindex $viewlist [expr $col-1]] 3]

	upvar #0 $dattab D
	$D(set) D $row $datcol $value
}

proc tab_viewget    { V row col } 	{
	upvar $V view;
	set viewlist $view(view)

	set dattab [lindex [lindex $viewlist [expr $col-1]] 0]
	set datnam [lindex [lindex $viewlist [expr $col-1]] 1]
	set datcol [lindex [lindex $viewlist [expr $col-1]] 3]

	if { [info exists view(selection)] 		\
	  && [string compare $view(selection) All]	\
	  && [string compare $view(selection) {}]} {
	    set datrow [lindex $view(selection) [expr $row-1]]
	} else {
	    set  datrow $row
	}

	if { ![string compare [string index $dattab 0] "!"] } {
	    set reply [eval  [string range $dattab 1 end] $datrow $col $datnam]
	} else {
	    upvar #0 $dattab D
	    if { [catch { $D(get) D $datrow $datcol } reply] } {
		return {}
	    }
	}
	return $reply
}
proc tab_viewcolnum { V name } { upvar $V view; return $view([string trim $name])		}
proc tab_viewcolname { V col } {
	upvar $V view

	lindex [lindex $view(view) [expr $col-1]] 1
}

proc tab_viewcoldel { V col }		{ }
proc tab_viewcolins { V name here }	{ }
proc tab_viewcolapp { V name }		{ }

proc tab_viewrowdel { V row }		{
	upvar $V view;

    foreach T $view(tables) {
	upvar #0 $T D;  $D(rowdel) D $row	
    }
    incr view(rows) -1
}
proc tab_viewrowins { V row }		{
	upvar $V view;

    foreach T $view(tables) {
	upvar #0 $T D;  $D(rowins) D $row	
    }
    incr view(rows)
}
proc tab_viewrowapp { V row }		{
	upvar $V view;

    foreach T $view(tables) {
	upvar #0 $T D;  $D(rowapp) D $row	
    }
    incr view(rows)
}

# Set up a tabview data array for use with tab
#
proc tabview_driver { Dr } {
	upvar $Dr driver

	set driver(nrows)	tab_viewnrows
	set driver(ncols)	tab_viewncols
	set driver(get)		tab_viewget
	set driver(set)		tab_viewset
	set driver(colname)	tab_viewcolname
	set driver(colnum)	tab_viewcolnum
	set driver(colins)	tab_viewcolins
	set driver(coldel)	tab_viewcoldel
	set driver(colapp)	tab_viewcolapp
	set driver(rowins)	tab_viewrowins
	set driver(rowdel)	tab_viewrowdel
	set driver(rowapp)	tab_viewrowapp
	set driver(columns)	tab_viewcolumns
}
tabview_driver TabView
