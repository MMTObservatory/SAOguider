
array set NullFileClass {
	file		{}
	directory	{}
	dirslist	{}
	multiple	 0
	blueplate	 1
	motif		 0
	loadtype	{}
	types {
    		{ "All files"               * }
	}
}

proc FileClassInit { Class } {
	upvar $Class class

    array set class [array get ::NullFileClass]
}

proc OpenFileDialog { parrent { fileclass NullFileClass } } {
        FileDialog $parrent tk_getOpenFile $fileclass
}
proc SaveFileDialog { parrent { fileclass NullFileClass } } {
        FileDialog $parrent tk_getSaveFile $fileclass
}

proc FileLast { Class filepath } {
    upvar #0 $Class class

    if { [llength $filepath] } {
        set file [lindex $filepath 0]
	set class(path) $file
	set class(file) [file tail $file]
	set class(directory) [file dirname $file]
    }
}

proc FileDialog { parent type class } {
        global FileDialogType
        global tk_strictMotif
        upvar #0 $class fileclass

    set title [string range $type 6 end]

    set tk_strictMotif $fileclass(motif)
 
    set filepath [$type                                \
                    -initialdir  $fileclass(directory) \
                    -initialfile $fileclass(file)      \
                    -filetypes   $fileclass(types)     \
                    -title       $title	               \
                    -blueplate   $fileclass(blueplate) \
                    -multiple    $fileclass(multiple)  \
                    -dirslist    $fileclass(dirslist)  \
		    -loadtype	 $fileclass(loadtype)  \
                    -parent	 $parent]

    FileLast $class $filepath
 
    return $filepath
}

