

age = 520 

bucket = {}

package = {
	["name"] = "hello",
	["id"] = 123,
	["age"] =30
}


--package = {1,2,3}
function init()
    print("init")
end


function dowork()
    print("dowork")
end

function finalize(...)
   print("finalize")
   return 100,200,300,400
end
