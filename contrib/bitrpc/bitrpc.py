from jsonrpc import ServiceProxy
import sys
import string

# ===== BEGIN USER SETTINGS =====
# if you do not set these you will be prompted for a password for every command
rpcuser = ""
rpcpass = ""
# ====== END USER SETTINGS ======


if rpcpass == "":
	access = ServiceProxy("http://127.0.0.1:8332")
else:
	access = ServiceProxy("http://"+rpcuser+":"+rpcpass+"@127.0.0.1:8332")
cmd = sys.argv[1].lower()

if cmd == "backupwallet":
	try:
		path = raw_input("Enter destination path/filename: ")
		print access.backupwallet(path)
	except:
		print "\n---An error occurred---\n"

elif cmd == "getaccount":
	try:
		addr = raw_input("Enter a Bitcoin address: ")
		print access.getaccount(addr)
	except:
		print "\n---An error occurred---\n"

elif cmd == "getaccountaddress":
	try:
		acct = raw_input("Enter an account name: ")
		print access.getaccountaddress(acct)
	except:
		print "\n---An error occurred---\n"

elif cmd == "getaddressesbyaccount":
	try:
		acct = raw_input("Enter an account name: ")
		print access.getaddressesbyaccount(acct)
	except:
		print "\n---An error occurred---\n"

elif cmd == "getbalance":
	try:
		acct = raw_input("Enter an account (optional): ")
		mc = raw_input("Minimum confirmations (optional): ")
		try:
			print access.getbalance(acct, mc)
		except:
			print access.getbalance()
	except:
		print "\n---An error occurred---\n"

elif cmd == "getblockbycount":
	try:
		height = raw_input("Height: ")
		print access.getblockbycount(height)
	except:
		print "\n---An error occurred---\