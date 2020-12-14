-- file: 'thanks.lua'
-- desc: to print the list of the contributing guys
function list_iter (t)
	local i = 0
	local n = table.getn(t)
	return function ()
		i = i + 1
		if i <= n then return t[i] end
	end
end

helpful_guys = {
	"----参与翻译----",
	"buxiu", "凤舞影天", "zhang3",
	"morler", "lambda", "sunlight",
	"\n",
	"----参与校对----",
	"凤舞影天", "doyle", "flicker",
	"花生魔人", "zhang3", "Kasi",
	"\n"
}

for e in list_iter(helpful_guys) do
	print(e)
end