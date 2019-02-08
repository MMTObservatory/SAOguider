option add *MMTIDialog*background yellow

proc busymessage_after { d } {
    if { ![winfo exists $d] } { return }
    catch { grab $d }  

    switch -- [wm state $d] {
        normal { raise $d }
        withdrawn -
        iconic {
            wm deiconify $d
        }
    }
    set rootx [winfo rootx .]
    set rooty [winfo rooty .]

    wm geometry $d +[expr $rootx+50]+[expr $rooty+250]

    after 2000 "busymessage_after $d"
}

proc Dialog_Create { top title args } {
    global dialog
    if [winfo exists $top] {
	switch -- [wm state $top] {
	    normal {
		# Raise a buried window
		raise $top
	    }
	    withdrawn -
	    iconic {
		# Open and restore geometry
		wm deiconify $top
		option add *background  yellow
		catch { wm geometry $top $dialog(geo,$top) }
	    }
	}
	return 0
    } else {
	eval {toplevel $top} $args -class MMTIDialog
	wm title $top $title
	return 1
    }
}

proc Dialog_Wait { top varName {focus {}} nbeeps } {
    upvar $varName var

    #puts "top: $top varName: $varName focus: $focus nbeeps: $nbeeps"
    busymessage_after $top

    # Poke the variable if the user nukes the window
    bind $top <Destroy> [list set $varName cancel]

    # Grab focus for the dialog
    if {[string length $focus] == 0 } {
	set focus $top
    }
    set old [focus -displayof $top]
    global dialogfocus
    set dialogfocus $focus
    focus $focus
    catch {tkwait visibility $top}
    #catch {grab $top}

    #raise $focus
    ringbell $nbeeps 15000 $nbeeps
# Wait for the dialog to complete
    tkwait variable $varName
    global ringbell
    catch { after cancel $ringbell }
    #catch {grab release $top}
#    focus $old
}

proc ringbell { nbeeps repeat thisbeep } {
	global ringbell dialogfocus
	#puts "dialog focus: $dialogfocus"
        focus $dialogfocus
	raise $dialogfocus
	if { $nbeeps > 0 } {
	    bell
	    if { $thisbeep == 1 } {
	        set ringbell [after $repeat "ringbell $nbeeps $repeat $nbeeps"]
	    } else {
	        set ringbell [after 1000 "ringbell $nbeeps $repeat [expr $thisbeep - 1]" ]
	    }
	}
}

proc Dialog_Dismiss { top } {
    global dialog
    # Save current size and position
    catch {
	# window may have been deleted
	#set dailog(geo,$top) [wm geometry $top]
	set dialog(geo,$top) [wm geometry $top]
	#wm withdraw $top
        bind $top <Destroy> {}
	destroy $top
    }
}

