## build
clone the repo (`git clone https://github.com/pozm/godot_key_extract.git`) then open the visual studio workspace and then build either release or debug, probably doesn't matter.


## how does this work?
basically when you want to build with encryption you must first compile your own godot template with the aes script key stored as an env variable (this automatically gets embedded into executable via the buildsystem), so the key is generated and stored in some random space in the executable, we can retrieve this via two different ways: either while it's in memory or while it's out of. The latter is much more complex and requires a custom disassembler, which i cannot be bothered writing.

so for retriving this from memory, we can find where the key is used, and where else better to look than the load bytecode function? So within this function it checks if the script even needs to be decrypted (via a ext check), and if it does then it does some steps, but most importantly it reads the secret key. we can now open up our favourite static analsys program (for example ida) and find this function, there are many ways to find this function but probably the easiest is to just search for strings.

Godot has a ton of macros and relies heavily on them for error handling, so we can easily find what we're looking for by searching just for "load_byte_code" and you will find the function we're looking for. now in the function it will do a ton of things, but the most important thing is loading the secret key. it does this with the opcode LEA, the function doesn't use it alot and it's pretty easy to tell which one is the secret key. so after we know the address of the instruction we can create a signature to it.

nd then in the dll we can use that signature to find the instruction while loaded in memory. after doing that all you need to do is read the instruction and calculate the offset. once done you should have the address to where the secret key is loaded in memory and you can just print it out.