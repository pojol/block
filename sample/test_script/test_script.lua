local test_module = {
	init = function() end,
	execute = function() end,
	shut = function() end,
}

test_module.init = function()
	print(1)
end

test_module.execute = function()
	print("hi")
end

test_module.shut = function()
	print(2)
end