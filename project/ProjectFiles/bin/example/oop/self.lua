Account = {balance = 0}
function Account.withdraw (v)
  Account.balance = Account.balance - v
end 

Account.withdraw(100.00)

a = Account; Account = nil
a.withdraw(100.00) -- ERROR!

--改进
function Account.withdraw (self, v)
  self.balance = self.balance - v
end

a1 = Account; Account = nil
a1.withdraw(a1, 100.00) -- OK

--应用于多个对象 
a2 = {balance=0, withdraw = Account.withdraw} 
a2.withdraw(a2, 260.00) 

--隐藏参数 
function Account:withdraw (v)
  self.balance = self.balance - v
end 

a:withdraw(100.00) 

--final & compare
Account = {
  balance=0,
  withdraw = function (self, v)
    self.balance = self.balance - v
  end
}

function Account:deposit (v)
  self.balance = self.balance + v
end

Account.deposit(Account, 200.00)
Account:withdraw(100.00) 
