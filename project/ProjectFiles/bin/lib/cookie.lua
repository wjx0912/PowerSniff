local Private = {}

--<==========================================================================>--
-- build data structures for parsing

-- create a lua regex expression to capture v in both uppercase/lowercase
function Private.create_match(v)
        local vU = string.upper(v)
        local vL = string.lower(v)
        local r = {}
        for i=1,string.len(v) do
                r[i]="["..string.char(string.byte(vU,i))..
                        string.char(string.byte(vL,i)).."]"
        end
        return "("..table.concat(r)..")"
end

-- fields of a cooker record
Private.cookie_av={
        "domain",
        "path",
        "comment",

--      "secure",
--      commented since the parser is too greedy and eats with
--      this rules also something like "aaa_secure" (plugin name).
--      not only a "secure" attribute.

        "version",
        "max-age",
        "expires"
}

-- some captures for the cookie fields
Private.value = {}
Private.value.token='=%s*("?[^";]*"?)'
Private.value.name="^(%s*[%w%_%-%.]+)"
Private.value.domain='=%s*("?%.?[%w%.%_%-%%%/%+%-%*]+"?)'
Private.value.expires="=%s*(%a+%s*,%s*[%w%:%s%-]+)"
Private.value.secure="(%s?)"
Private.value.version="=%s*(%d)"

Private.left_table = {}

-- puts in left_table the map: name -> expression_to_capture_name_value
-- used for catching the left part of an equality,
-- consider that the left_table["domain"] will capture
-- "([Dd][Oo][Mm][Aa][Ii][Nn])" that will be used to find the first part of
-- a string like:
-- DOMAIN = VALUE
table.foreach(Private.cookie_av,function(_,v)
        Private.left_table[v] = Private.create_match(v)
        end)
Private.left_table["name"]=Private.value.name

-- eat generates functions that will be called on the cookie string s and
-- the storage table t, these eating functions will put in
-- t[name] the capture
function Private.eat(name,value)
        return function(s,t)
                local b,e,l,r =
                        string.find(s,Private.left_table[name].."%s*"..value)
                if b then
                        t[name]= r
                        return string.sub(s,1,b-1)..string.sub(s,e+1,-1)
                end
                return s
        end
end

function Private.eat2(name1,name2,value)
        return function(s,t)
                local b,e,l,r =
                        string.find(s,Private.left_table[name1].."%s*"..value)
                if b then
                        t[name1]=l
                        t[name2]=r
                        return string.sub(s,1,b-1)..string.sub(s,e+1,-1)
                end
                return s
        end
end

-- temp table used to build the following one
Private.types = {}
table.foreach(Private.cookie_av,function(_,v)
        Private.types[v] = Private.value[v] or Private.value["token"]
        end)
Private.types["name"] = Private.value["token"]

-- this table will contains the eaters funcions for all the elements in the
-- cookie grammar, see cookie_av
Private.syntax = {}
table.foreach(Private.cookie_av,function(_,v)
        Private.syntax[v] = Private.eat(v,Private.types[v])
        end)
Private.syntax["name"] = Private.eat2("name","value",Private.types["name"])

--<==========================================================================>--
-- helper functions

-- given a host h and a string s, this returns a table with all the cookie
-- fields plus host h
function Private.parse_cookie(s,h)
        local t = {}
        local s1 = s
        table.foreach(Private.syntax,function(n,f) s1 = f(s1,t) end)
        if t.expires ~= nil then
            -- Fix to deal with a date expiration that is larger than our data structure can handle.  This needs to be revisited
                -- in at least 2019.
                --
        t.expires = string.gsub(t.expires, "20[4-9]+", "2020")
                local tmp = tonumber(t.expires)
                t["expires-raw"] = t["expires"]
                if tmp ~= -1 then
                        t["expires"] = tmp
                end
        end
        if t.domain == nil then
                if h == nil then
                        error("parse_cookies cant be called with h = nil")
                end
                t.domain = h
        end
        t.timestamp = os.time()
        t.host = h
        return t
end

 -- checks if the cookie has to be purged
 function Private.is_expired(c)
     local date = os.time()

     if c["expires"] ~= nil then
         if c["expires"] < date then
             return true
         end
     end

     if c["max-age"] then
         if c["max-age"] > date then
             return true
                end
        end

        -- this should not be necessary, but...
        if c["expires"] ~= nil and type(c["expires"]) == "string" then
                c["expires"] = tonumber(c["expires"])
        end

        if c["expires"] and c["timestamp"] then
                if os.difftime(date,c["timestamp"]) > c["expires"] then
                        return true
                end
        end
        return false
end

-- puts a uri in a table containing the / separated parts
function Private.split_uri(path)
        local t = {}
        if not path then return t end
        for w in string.gfind(path,"/[%w%%%_%.%~]*") do
                table.insert(t,string.sub(w,2,-1))
        end
        return t
end

-- counts how many parts of path are in common
function Private.subpath_len(path,sub)
        if not sub then
                return -1 --no path was
        end
        --print(path,sub)
        local t1 = Private.split_uri(path)
        local t2 = Private.split_uri(sub)
        local n = 0
        local function count(i,v)
                if t2[i] == v then
                        n = n + 1
                else
                        return false
                end
        end
        table.foreachi(t1,count)
        return n
end

--<==========================================================================>--

-- cleans expired cookies
local function clean_expired(t)
		if t == nil then 
			return
		end
		
        table.foreach(t,function(x,c)
        if Private.is_expired(c) then
                t[x] = nil
        end
        end)
end

-- parse
local function parse_cookies(s,h)
        if s then
                local r = {}
                table.insert(r,Private.parse_cookie(s,h))
                return r
        else
                return nil
        end
end

-- merges two tables of cookies
local function merge(t2,t1)
        if not t1 then
                return
        end
        clean_expired(t1)
        clean_expired(t2)
        table.foreach(t1,function(_,c)
                local match = 0
                table.foreach(t2,function(_,c2)
                        if c["name"] == c2["name"] and
                           c["domain"] == c2["domain"] then
                                table.foreach(c,function(n,_)
                                        c2[n] = c[n] or c2[n] -- fix shit
                                end)
                                match = 1
                                return 1 -- exit loop
                        end
                        end)
                if match == 0 then
                        -- add it to the c
                        table.insert(t2,c)
                end
        end)
end

-- returns the needed cookie for the domain...
-- returns the string
local function get(t,res,domain,host)
        if domain == nil then
                error("cookie.get can't be called with an empty domain")
        end
      --print("cookie.get("..(res or"nil")..(domain or"nil")..(host or"nil"))
        local function domain_match(a,b)
                local x,_ = string.find(a,b)
                --print("Find: '"..a.."' '"..b.."' = "..tostring(x ~= nil))
                return x ~= nil
        end
        local r = {}
      --print(res,domain,host)
        table.foreach(t,function(_,c)
      --table.foreach(c,print)
                local l = Private.subpath_len(res,c["path"])
              --print(Private.is_expired(c),l,c["domain"],c.host)
                if not Private.is_expired(c) and
                   l >= 0 and
                   (domain_match("."..domain,c["domain"]) or c.host == host)
                        then
                        --search if there is no othe cookie in r
                        --with the same name
                        local dup = false
                        table.foreachi(r,function(_,v)
                                if v.c.name == c.name then dup = true end
                        end)
                        if not dup then
                                table.insert(r,{c=c,l=l})
                        end
                end
        end)
        table.sort(r,function(a,b) return a.l > b.l end)
        local s = ""
        table.foreach(r,function(_,w)
                if w.c.name then
                        s = s .. "; " .. w.c.name .. "=" .. w.c.value
                end
        --      if w.c.domain then
        --              s = s .. '; $Domain = "' .. w.c.domain .. '"'
        --      end
        --      if w.c.path then
        --              s = s .. '; $Path = "' .. w.c.path .. '"'
        --      end
        end)
        if s ~= "" then
                --return '$Version = "1" ' .. s
                return string.sub(s,2)
        else
                return nil
        end
end


-- declare local variables
--// exportstring( string )
--// returns a "Lua" portable version of the string
local function exportstring( s )
  s = string.format( "%q",s )
  -- to replace
  s = string.gsub( s,"\\\n","\\n" )
  s = string.gsub( s,"\r","\\r" )
  s = string.gsub( s,string.char(26),"\"..string.char(26)..\"" )
  return s
end

--// The Save Function
local function savetable( tbl,filename )
   local charS,charE = "   ","\n"
   local file,err
   -- create a pseudo file that writes to a string and return the string
   if not filename then
      file =  { write = function( self,newstr ) self.str = self.str..newstr end, str = "" }
      charS,charE = "",""
   -- write table to tmpfile
   elseif filename == true or filename == 1 then
      charS,charE,file = "","",io.tmpfile()
   -- write table to file
   -- use io.open here rather than io.output, since in windows when clicking on a file opened with io.output will create an error
   else
      file,err = io.open( filename, "w" )
      if err then return _,err end
   end
   -- initiate variables for save procedure
   local tables,lookup = { tbl },{ [tbl] = 1 }
   file:write( "return {"..charE )
   for idx,t in ipairs( tables ) do
      if filename and filename ~= true and filename ~= 1 then
         file:write( "-- Table: {"..idx.."}"..charE )
      end
      file:write( "{"..charE )
      local thandled = {}
      for i,v in ipairs( t ) do
         thandled[i] = true
         -- escape functions and userdata
         if type( v ) ~= "userdata" then
            -- only handle value
            if type( v ) == "table" then
               if not lookup[v] then
                  table.insert( tables, v )
                  lookup[v] = #tables
               end
               file:write( charS.."{"..lookup[v].."},"..charE )
            elseif type( v ) == "function" then
               file:write( charS.."loadstring("..exportstring(string.dump( v )).."),"..charE )
            else
               local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
               file:write(  charS..value..","..charE )
            end
         end
      end
      for i,v in pairs( t ) do
         -- escape functions and userdata
         if (not thandled[i]) and type( v ) ~= "userdata" then
            -- handle index
            if type( i ) == "table" then
               if not lookup[i] then
                  table.insert( tables,i )
                  lookup[i] = #tables
               end
               file:write( charS.."[{"..lookup[i].."}]=" )
            else
               local index = ( type( i ) == "string" and "["..exportstring( i ).."]" ) or string.format( "[%d]",i )
               file:write( charS..index.."=" )
            end
            -- handle value
            if type( v ) == "table" then
               if not lookup[v] then
                  table.insert( tables,v )
                  lookup[v] = #tables
               end
               file:write( "{"..lookup[v].."},"..charE )
            elseif type( v ) == "function" then
               file:write( "loadstring("..exportstring(string.dump( v )).."),"..charE )
            else
               local value =  ( type( v ) == "string" and exportstring( v ) ) or tostring( v )
               file:write( value..","..charE )
            end
         end
      end
      file:write( "},"..charE )
   end
   file:write( "}" )
   -- Return Values
   -- return stringtable from string
   if not filename then
      -- set marker for stringtable
      return file.str.."--|"
   -- return stringttable from file
   elseif filename == true or filename == 1 then
      file:seek ( "set" )
      -- no need to close file, it gets closed and removed automatically
      -- set marker for stringtable
      return file:read( "*a" ).."--|"
   -- close file and return 1
   else
      file:close()
      return 1
   end
end

--// The Load Function
local function loadtable( sfile )
   -- catch marker for stringtable
   if string.sub( sfile,-3,-1 ) == "--|" then
      tables,err = loadstring( sfile )
   else
      tables,err = loadfile( sfile )
   end   
   if not tables and err then 
		return nil
   end
   tables = tables()
   for idx = 1,#tables do
      local tolinkv,tolinki = {},{}
      for i,v in pairs( tables[idx] ) do
         if type( v ) == "table" and tables[v[1]] then
            table.insert( tolinkv,{ i,tables[v[1]] } )
         end
         if type( i ) == "table" and tables[i[1]] then
            table.insert( tolinki,{ i,tables[i[1]] } )
         end
      end
      -- link values, first due to possible changes of indices
      for _,v in ipairs( tolinkv ) do
         tables[idx][v[1]] = v[2]
      end
      -- link indices
      for _,v in ipairs( tolinki ) do
         tables[idx][v[2]],tables[idx][v[1]] =  tables[idx][v[1]],nil
      end
   end
   return tables[1]
end


module("cookie",package.seeall)

function cookiefilepath(domain)
	dir = os.getenv("APPDATA") or "/tmp/"
	
	dir = dir .. "/Anvsoft/"
	if string.byte(domain) == 46 then
		return dir .. string.sub(domain, 1-string.len(domain));
	else
		return dir .. domain;
	end
end

function loadcookie(domain, path)
	local cookies = {}
	local subdomain = domain
	while string.match(subdomain, ".-%..-$") do
		local c = loadtable(cookiefilepath(subdomain))
		--print("load cookie",cookiefilepath(subdomain),c,cookies)
		if c then 
			merge(cookies, c)
		end
		
		subdomain = string.match(subdomain, ".-%.(.*)$")
	end		
	
	local d = string.match(domain, ".+%.(.-%..-)$") or domain
	if cookies then
		return get(cookies, path, d, domain)
	else
		return nil
	end
end

function savecookie(cookiestring, domain)
	if not cookiestring then return end
	--print("save cookie",domain, cookiestring)
	local cookies = {}
	for c in string.gmatch(cookiestring, "{{(.-)}}") do
		local tmp = parse_cookies(c, domain)
		merge(cookies,tmp)
	end
	
	if cookies ~= {} then
		local domains = {}
		for k,v in pairs(cookies) do
			if not domains[v.domain] then
				domains[v.domain] = {}
			end
			table.insert(domains[v.domain], v)			
		end
		
		for k,v in pairs(domains) do
			local exist = loadtable(cookiefilepath(k));
			if exist then
				merge(exist, v)
			else
				exist = v
			end
			
			savetable(exist, cookiefilepath(k))
		end
	end
end
