-- This script is used for cards lua tests --

cards.fs.rm("test")
print("Creating test")

local f = io.open("test", "w")
f:write("hello this a small test \n")
f:write("With a few lines \n")
f:close()

cards.fs.cp("test", "test2")
cards.fs.rm("test")
cards.fs.ln("test2", "test")
cards.fs.mv("test", "test1")

