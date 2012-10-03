Quick Readme.txt for Dynamic BASS.DLL wrapper by Rogier Timmermans 
(MagicRT@hotmail.com)
-------------------------------------------------------------------------------------------------------------------------------------------------

Contents :
----------

1. What is it ?
1a. Why is it ?
1b. Why load DLL's in a dynamic fashion ?
2. How to install & use it in Delphi 3.x and up.
2a. Adding more functions.
3. Tips & Tricks.
3a. How to do the Resource-Thing !
4. Credits & stuff.


1. What is it ?
---------------
The Dynamic BASS.DLL wrapper is a delphi wrapper for the BASS.DLL. from 
Un4seen productions.
In addition to the common functionality that the Dll offers, this wrapper 
has the added advantage of dynamic dll loading, which
simply means that the dll doesn't have to be present at runtime (at least at 
your app's startup-time, you DO need it to have it
when you want to use it, of course !). The application does need a bit more 
"protection", when using this dynamic wrapper
instead of the static wrapper (the true advantages will become clear if you 
continue to read this readme...duh !), but the
overhead is actually better for your application...the more protection, the 
safer it is...

1a. Why is it ?
---------------------
I (RT) had found and tried bassmod.dll, and since I'm used to link my dll's 
in a dynamic fashion (at least the non-standard
Win9x ones) and I had nothing better to do that week, I wrote the dynamic 
API delphi for bassmod, borrowing a lot of code
from the static version  that was included with bassmod.

A few days later I decided to share it with the world and send the "beta" 
version of this wrapper to Ian, where it was placed
on the web-site. However, this wrapper contained only those functions that I 
needed and was, by far, complete.

Anyway, to make a long story short, I've added support for every function 
from the Dll which lead to the final version for both
BASS and BASSMOD (the BASS wrapper was simply a bit more cut & paste work, 
the BASMOD wrapper provided the
"framework" for this wrapper)...and here it is !

I was thinking of adding a number of boolean parameters to enable loading of 
specific function-groups only (ea : EAX related, synchronisers
related etc...you don't need these if you just want to play a MOD in your 
app)...maybe I'll add them later...or maybe not...;-) !
If you send me some feedback, I might think about it...


1b. Why load DLL's in a dynamic fashion ?
-----------------------------------------
DLL's are dynamic link libraries by design, which means that they can be 
linked in at anytime before being used and unloaded.
In Delphi/ Windows there are two ways of linking DLL's : Static and Dynamic.

If you've ever seen an application bail while returning "xxx.dll not found" 
at startup, you have witnessed a non-dynamic
(also called static) link of a dynamic link library (hey, this is NOT why 
they were created, so lets all use the dynamic abilities !).
Static linking simply means that a bit of code is linked into your app that 
checks for & "autoloads" the dll. Dynamic linking is, although
a bit more difficult to use, far better. Why ?

Well, some advantages of dynamic linking can be summed up...:

* Conserve memory (load & unload what you need, when you need it or no 
longer need it)
* Dll Supply (you can hide the dll somewhere ; you can perform some pretty 
nasty tricks !)
* App runs without DLL (only the functions from the DLL are not working if 
it's missing)

The difference in coding is that you have to supply that auto-generated DLL 
link-in code and make sure you get rid of
everything you link in...that's all !


2. How to install & use it in Delphi 3.x and up.
------------------------------------------------
Note : I haven't tested this in Delphi 5, if anybody can send me Delphi 5 
Pro or Standard (No demos please !), I will though...

- Simple, add the unit to your uses clause and call the functions.
- Check out the demo apps for a basic example & more info on how to use it !
- Check out the static API version that's also included with bass for more 
info.

   just remember :

    - call load_bassdll() before anything else; this links in the dll's 
functions.
    - use the dll's functions (the linked in functions have the RT_xxx 
prefix...;-) ! );
    - use "try...finally" blocks to shield "hazardous" actions
    - deinit everything you init or put otherwise : know where & how much 
memory you're claiming and make sure it gets freed afterwards...
    - be creative, but carefull !
    - call unload_bassdll() after you're done with it...this get's rid of 
everything; You're not required to call RT_BASS_FREE();
      this is done automatically, but just call it before calling 
UNLOAD_BASSDLL(), this is called programming etiquette...and
      by calling it, you're sure where it's cleared !


2a. Adding more functions.
---------------------------------------

- Open the unit in Delphi, there's information in there on how to import 
more functions from bass.dll (should the need arise) !

in short :
  1) Add the variable declaration (see source),
  2) Add the fucntion link-in (see source),
  3) Check if link-in was succesfull (same here).


3. Tips & Tricks.
------------------------

Since BASS is not a standard windows dll, you have to supply it with your 
application !

The statically linked version required the dll to be available (somewhere in 
the path) on app-start, but since we're using a dynamic link,
(yes, here comes the cool part...) it doesn't have to be !

Imagine this :
-------------------
You're writing an application that will be burned onto a CD (I have !). Some 
joker copies the EXE to HD and he 'forgets' to copy the DLL (or he
removes the CD from the drive while the app is loaded and inserts disc 
#2...this bites serious wind, since Windows has the decency to present
you with a nice blue screen of death !). If you were using the static 
version, you would be in trouble !
If you however use the dynamic version, you can simply include the DLL as a 
resource, stream it out to the windows temp-dir and
load it without ever receiving emails from 'less intelligent' users who are 
unable to leave the CD in the drive...!
(well, this doesn't solve our disc changing problem, but there are other 
tricks for that one...it was just meant as an illustration !)


3a. How to do the Resource-Thing !
----------------------------------

Here's an easy way to do this :

1) create an RC script and compile the DLL into a RES file.
    I'd recommend "Resource Builder" (try : http://www.votum.md/sicmps ) for 
people who are new to this...
    there are a few weird bugs in there, but it works.
2) link in this RES ea :  {$R file.res}
3) on - project startup - (that's before the first formcreate, in the 
"project" source !) !
   create a TResourcestream, load in the dll-resource and stream it out to 
the windows temp-dir.
   Please note that a random, non existing, file or directroy as stream 
target is used best, although you could check for
   existance (and delete it) before streaming it out, but you could come 
into conflict with other files...here's one you have
   to solve for yourself...my method always works btw ! anybody want info ?
4) dll streamed out ->  kick the TResourcestream, ready to go...
5) create the forms, do whatever you want now, the dll can be loaded in at 
anytime since it's there now...

note : the streaming can be performed at anytime also !

tip : compress & crypt the resources of your app, this saves loads of space 
and has other advantages too !

If you don't know how to perform some of these steps, let me know, I'll send 
you an example...


4. Credits & stuff.
-------------------

BASS.DLL - Ian Luck  / "www.un4seen.com" - crew
Static Delphi API DLL wrapper - Titus Miloi
Dynamic Delphi API DLL wrapper - Rogier Timmermans (that's me !)

If you want dynamic support for Unrar.dll and Unace.dll in your apps, I have 
written dynamic wrappers for these dlls too.
Mail me if you want them ! If you use the wrapper, some credits in your app 
would be nice.

Thanks to Ian for the DLL and Titus for the Static API.

Thank you for using it.

ps :  If you get rich by using any of my dynamic components, please feel 
free to send me a lot of money !

RT.

