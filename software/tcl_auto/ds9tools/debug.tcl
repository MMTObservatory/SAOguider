
if { ![catch {
	global env
	set x $env(DS9DEBUG)
   }] } {
	catch { rename tkerror {} }
}

