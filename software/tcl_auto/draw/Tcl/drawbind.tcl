
bind Draw <ButtonPress-1> { %W beginpan %x %y		}
bind Draw <ButtonRelease> { %W endpan			}
bind Draw <B1-Motion>	  { %W pan %x %y		}
bind Draw <Enter> 	  { focus %W			}

bind Draw <z> 		{ %W zoom about %x %y 1.25 1.25 }
bind Draw <Z> 		{ %W zoom about %x %y 2.00 2.00 }
bind Draw <x> 		{ %W zoom about %x %y 0.80 0.80 }
bind Draw <X> 		{ %W zoom about %x %y 0.50 0.50 }
bind Draw <Key-1>	{ %W zoom about %x %y		}

bind Draw <h>		{ %W pan  %%-1 +0		}
bind Draw <j>		{ %W pan  %%+0 -1		}
bind Draw <k>		{ %W pan  %%+0 +1		}
bind Draw <l>		{ %W pan  %%+1 +0		}

bind Draw <H>		{ %W pan  %%-5 +0		}
bind Draw <J>		{ %W pan  %%+0 -5		}
bind Draw <K>		{ %W pan  %%+0 +5		}
bind Draw <L>		{ %W pan  %%+5 +0		}
