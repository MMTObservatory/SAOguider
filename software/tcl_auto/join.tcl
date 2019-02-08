# The join is an array where the indicies are the values of the 
# j2 column.  The value at each index is the row in t2 where that 
# value occures.  This implements a look up table to join rows, 
# not a true join.
#
#
#
proc ted_joincreate { J data datac1 join joinc2 } {
	upvar #0 $J join

	set J(data) 	$data
	set J(datac1)   $datac1
	set J(join) 	$join
	set J(joinc2)   $joinc2

	set J(datac1num) [ted_datacolnum $data $datac1]
	set J(joinc2num) [ted_datacolnum $join $joinc2]

	set nrows [ted_nrows $data]

	for { set row 1 } { $row < $nrows } { incr row } {
	    set join(lookup.[ted_dataget $data $row $joinc2]) $row
	}
}

proc tedjoin_maprow  { data row } {
	upvar #0 $D data

	return join(lookup.[ted_dataget $join(data) $row $join(joinc2num)])
}


# Table driver wrappers
#
proc ted_joinnrows  { V }		{ upvar #0 $V join; ted_datanrows $join(data)		}
proc ted_joinncols  { V }		{ upvar #0 $V join; llength $join(join)			}
proc ted_joinset    { V row col value }	{
	upvar #0 $V join;
	ted_dataset [tedjoin_mapdat $join(join) $col] $row [tedjoin_mapcol $join(join) $col] $value
}
proc ted_joinget    { V row col } 	{
	upvar #0 $V join;
	ted_dataget [tedjoin_mapdat $join(join) $col] $row [tedjoin_mapcol $join(join) $col]
}
proc ted_joincolnum { V name } 		{ upvar #0 $V join; ted_datacolnum  $join(data) $name	}
proc ted_joincolname { E col } {
	upvar #0 $E editor

	lindex [lindex $editor(join) [expr $col-1]] 1
}
proc ted_joincoldel { V col }		{ upvar #0 $V join; ted_datacoldel $join(data) $col		}
proc ted_joincolins { V name here }	{ upvar #0 $V join; ted_datacolins $join(data) $name $here	}
proc ted_joincolapp { V name }		{ upvar #0 $V join; ted_datacolapp $join(data) $name		}
proc ted_joinrowdel { V row }		{ upvar #0 $V join; ted_datarowdel $join(data) $row		}
proc ted_joinrowins { V row here }	{ upvar #0 $V join; ted_datarowins $join(data) $row $here	}
proc ted_joinrowapp { V row }		{ upvar #0 $V join; ted_datarowapp $join(data) $row		}

# Set up a tedjoin data array for use with ted
#
proc tedjoin_driver { Dr } {
	upvar $Dr driver

	set driver(nrows)	ted_joinnrows
	set driver(ncols)	ted_joinncols
	set driver(get)		ted_joinget
	set driver(set)		ted_joinset
	set driver(colname)	ted_joincolname
	set driver(colnum)	ted_joincolnum
	set driver(colins)	ted_joincolins
	set driver(coldel)	ted_joincoldel
	set driver(colapp)	ted_joincolapp
	set driver(rowins)	ted_joinrowins
	set driver(rowdel)	ted_joinrowdel
	set driver(rowapp)	ted_joinrowapp
}
tedjoin_driver TedJoin

