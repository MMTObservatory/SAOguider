# - - - - - - - - - - - - - - - - - - - - - - - - - - - #
# function: setlist
# purpose : Distribute an undetermined number of list data
#           over a set of pre-determined variables.
# design  : Take variable names and a list. Set the names to
#           indexed values in the list; with overflow in largs
#           - if largs is the last variable. Variables may give
#           default values, used if the value list runs short.
# usage   : setlist {one two largs} {A B C D}; puts "$one $largs" 
#           setlist {eks why zee}   {A B C D}; puts "$eks $zee" 
#           setlist {a {b Y} {c Z}} {A B}    ; puts "$a $b $c"
# - - - - - - - - - - - - - - - - - - - - - - - - - - - #
# 21jul2005 I know this is very C-style programming. In ver 1.1
#           where I did "foreach n nList v vList {...}" it was not
#           possible to tell the end of vList from "" assignments.
package provide setlist 1.2

proc setlist {nList vList} {

	set nLength [llength $nList]
	set vLength [llength $vList]

	# Run through the name list, assigning values.
	for {set i 0} {$i < $nLength} {incr i} {
		set n [lindex $nList $i]
		set v [lindex $vList $i]

		# If 'n' has a default value, eg: {fName foo.txt}
		catch {unset nDef}

		if {[llength $n] == 2} {
			set nDef [lindex $n 1]
			set n    [lindex $n 0]

			upvar $n nVar
			set nVar $nDef
		}

		# If there's a matching 'v', set 'n'.
		if {$i < $vLength} {
			upvar $n nVar
			set nVar $v

		# Otherwise, if a default wasn't set, error.
		} elseif {![info exists nDef]} {
			error "List too short to set '$n'."
		}
	}

	# If there's more of vList, and the last element
	# of nList was 'largs', append the rest to 'largs'.
	if {$i < $vLength
	&&  [string equal $n largs]} {
		#lappend $nVar [lrange $vList $i end] ;#!
		set nVar [concat $nVar [lrange $vList $i end]]
	}
}
